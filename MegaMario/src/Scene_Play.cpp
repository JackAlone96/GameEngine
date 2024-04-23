#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), m_levelPath(levelPath)
{
	Init(m_levelPath);
}

void Scene_Play::Init(const std::string& levelPath)
{
	RegisterAction(sf::Keyboard::P, "PAUSE");
	RegisterAction(sf::Keyboard::Escape, "QUIT");
	RegisterAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	RegisterAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	RegisterAction(sf::Keyboard::G, "TOGGLE_GRID");

	//TODO: Register all other gameplay Actions

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().GetFont("Tech"));

	//LoadLevel(levelPath); // TODO: ???
}

Vec2 Scene_Play::GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	// TODO: This function takes in a grid (x,y) position and an Entity
	//		 Return a Vec2 indicating where the CENTER position of the Entity should be
	//		 You must use the Entity's Animation size to position it correctly
	//		 The size of the grid width and height is stored in m_gridSize.x and m_gridSize.Y
	//		 The bottom-left corner of the Animation should align with the bottom left of the grid cell

	return Vec2(0, 0);
}

void Scene_Play::LoadLevel(const std::string& filename)
{
	// reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	// TODO: read in the level file and add the appropriate entities
	//		 use the PlayerConfig struct to store player properties
	//		 this struct is defined ad the top of Scene_Play.h

	// NOTE all of the code below is sample code which shows you how to set up and use entities with the new syntax, it should be removed

	SpawnPlayer();

	// some sample entities
	auto brick = m_entityManager.AddEntity("tile");
	// IMPORTANT: always add the CAnimation component first so that GridToMidPixel can compute correctly
	brick->AddComponent<CAnimation>(m_game->assets().GetAnimation("Brick"), true);
	brick->AddComponent<CTransform>(Vec2(96, 480));

	// NOTE: Your final code should position the entity with the grid x,y position read from the file:
	// brick->AddComponent<CTransform>(GridToMidPixel(gridX, gridY, brick));

	if (brick->GetComponent<CAnimation>().animation.getName() == "Brick")
	{
		std::cout << "This could be a good way of identifying if a tile is a brick!" << std::endl;
	}

	// NOTE: INCREDIBLY IMPORTANT
	//		 Components are now returned as references rather than pointers
	//		 If you don't specify a reference variable type it will COPY the component

	// COPY EXAMPLE
	// auto transform1 = entity->Get<CTranform>()

	// REFERENCE EXAMPLE
	// auto& transform2 = entity->get<CTransform>()
}

void Scene_Play::SpawnPlayer()
{
	// sample player
	m_player = m_entityManager.AddEntity("player");
	m_player->AddComponent<CAnimation>(m_game->assets().GetAnimation("Stand"), true);
	m_player->AddComponent<CTransform>(Vec2(224, 352));
	m_player->AddComponent<CBoundingBox>(Vec2(48, 48));
	m_player->AddComponent<CGravity>(0.1);

	// TODO: Add the remaining component to the player
}

void Scene_Play::SpawnBullet()
{
	// TODO: this should spawn a bullet at the given entity, going in the diretion the entity is facing
}

void Scene_Play::Update()
{
	m_entityManager.Update();

	// TODO: Implement pause functionality

	SMovement();
	SLifespan();
	SCollision();
	SAnimation();
	SRenderer();
}

void Scene_Play::SMovement()
{
	Vec2 playerVelocity(0, m_player->GetComponent<CTransform>().velocity.y);

	if (m_player->GetComponent<CInput>().up)
	{
		// Change palyer state
		playerVelocity.y = -3;
	}

	m_player->GetComponent<CTransform>().velocity = playerVelocity;

	for (auto e : m_entityManager.GetEntities())
	{
		if (e->HasComponent<CGravity>())
		{
			e->GetComponent<CTransform>().velocity.y += e->GetComponent<CGravity>().gravity;

			// if the player is moving faster than max speed in any direction set its speed in that direction to the max speed
		}

		e->GetComponent<CTransform>().pos += e->GetComponent<CTransform>().velocity;
	}

	// TODO: Implement player movement/jumping based on its CInput component
	// TODO: Implement gravity's effect on the player
	// TODO: Implement the maximum player speed in both X and Y directions
	// NOTE: Setting an entity's scale.x to -1/1 will make it face to the left/right
}

void Scene_Play::SEnemySpawner()
{
}

void Scene_Play::SRenderer()
{
	// color the background darker so you know that the game is paused
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	// set the viewport of the window to be centered on the player if it's far enough right
	auto& pPos = m_player->GetComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.GetEntities())
		{
			auto& transform = e->GetComponent<CTransform>();

			if (e->HasComponent<CAnimation>())
			{
				auto& animation = e->GetComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		for (auto e : m_entityManager.GetEntities())
		{
			if (e->HasComponent<CBoundingBox>())
			{
				auto& box = e->GetComponent<CBoundingBox>();
				auto& transform = e->GetComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0,0,0,0));
				rect.setOutlineColor(sf::Color(255, 255, 255, 255));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	// draw the grid for debugging purposes
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - Width() / 2;
		float rightX = leftX + Width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			DrawLine(Vec2(x, 0), Vec2(x, Height()));
		}

		for (float y = 0; y < Height(); y += m_gridSize.y)
		{
			DrawLine(Vec2(leftX, Height() - y), Vec2(rightX, Height() - y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, Height() - y - m_gridSize.y + 2);
				m_game->window().draw(m_gridText);
			}
		}
	}
}

void Scene_Play::SDebug()
{
}

void Scene_Play::SLifespan()
{
	// TODO: Check lifespan of entities that have them, and destroy them if they go over
}

void Scene_Play::SCollision()
{
	// REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
	//			 This means jumping will have a negative y-component
	//			 and gravity will have a positve one

	// TODO: Implement Physics::GetOverlap() function here

	// TODO: Implement bullet/tile collisions
	//		 Destroy the tile if it has a Brick animation
	// TODO: Implement player/tile collisions and resolutions
	//		 Update the CState component of the plauer to store wether it is currently on the ground or in the air
	//		 This will be used by the animation system
	// TODO: Check to see if the plauer has fallen down a hole (y < height())
	// TODO: Don't let the player walk off the left side of the map
}

void Scene_Play::SDoAction(const Action& action)
{
	if (action.Type() == "START")
	{
		if (action.Name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.Name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.Name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.Name() == "PAUSE") { SetPaused(!m_paused); }
		else if (action.Name() == "QUIT") { OnEnd(); }
		else if (action.Name() == "UP")
		{
			m_player->GetComponent<CInput>().up = true;
		}
	}
	else if (action.Type() == "END")
	{
		if (action.Name() == "UP")
		{
			m_player->GetComponent<CInput>().up = false;
		}
	}
}

void Scene_Play::SAnimation()
{
	// TODO: Complete the Animation class code first

	if (m_player->GetComponent<CState>().state == "air")
	{
		m_player->AddComponent<CAnimation>(m_game->assets().GetAnimation("Air"), true);
	}

	// TODO: set the animation of the player based on its CState component
	// TODO: for each entity with an animation, call entity->GetComponent<CAnimation>().animation.Update()
	//		 if the animation is not repeated and it has ended, destroy the entity
}

void Scene_Play::OnEnd()
{
	// TODO: When the scene ends, change bach to the MENU scene
}
