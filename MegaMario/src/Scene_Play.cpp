#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

Scene_Play::Scene_Play() {}

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

	RegisterAction(sf::Keyboard::W, "JUMP");
	RegisterAction(sf::Keyboard::D, "WALK-R");
	RegisterAction(sf::Keyboard::A, "WALK-L");
	RegisterAction(sf::Keyboard::Space, "FIRE");

	//TODO: Register all other gameplay Actions

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().GetFont("Tech"));

	LoadLevel(levelPath);
}

Vec2 Scene_Play::GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	// This function takes in a grid (x,y) position and an Entity
	//urn a Vec2 indicating where the CENTER position of the Entity should be
	// must use the Entity's Animation size to position it correctly
	// size of the grid width and height is stored in m_gridSize.x and m_gridSize.Y
	// bottom-left corner of the Animation should align with the bottom left of the grid cell
	const Vec2& entitySize = entity->GetComponent<CAnimation>().animation.getSize();
	Vec2 centerPosition = Vec2(gridX * m_gridSize.x, gridY * m_gridSize.y) + entitySize/2;
	centerPosition.y = m_game->window().getSize().y - centerPosition.y;

	return centerPosition;
}

void Scene_Play::LoadLevel(const std::string& filename)
{
	// reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	// TODO: read in the level file and add the appropriate entities
	//		 use the PlayerConfig struct to store player properties
	//		 this struct is defined ad the top of Scene_Play.h
	std::fstream fin(filename);
	std::string label;

	while (fin >> label)
	{
		if (label == "Tile")
		{
			std::string animationName;
			Vec2 position;

			fin >> animationName;
			fin >> position.x;
			fin >> position.y;

			auto tile = m_entityManager.AddEntity("tile");
			tile->AddComponent<CAnimation>(m_game->assets().GetAnimation(animationName), true);
			tile->AddComponent<CTransform>(GridToMidPixel(position.x, position.y, tile));
			tile->AddComponent<CBoundingBox>(tile->GetComponent<CAnimation>().animation.getSize());
		}
		else if (label == "Player")
		{
			fin >> m_playerConfig.X;
			fin >> m_playerConfig.Y;
			fin >> m_playerConfig.CX;
			fin >> m_playerConfig.CY;
			fin >> m_playerConfig.ACCELERATION;
			fin >> m_playerConfig.JUMP;
			fin >> m_playerConfig.MAXSPEED;
			fin >> m_playerConfig.GRAVITY;
			fin >> m_playerConfig.WEAPON;
		}
	}

	// NOTE all of the code below is sample code which shows you how to set up and use entities with the new syntax, it should be removed

	SpawnPlayer();

	//// some sample entities
	//auto brick = m_entityManager.AddEntity("tile");
	//// IMPORTANT: always add the CAnimation component first so that GridToMidPixel can compute correctly
	//brick->AddComponent<CAnimation>(m_game->assets().GetAnimation("Brick"), true);
	//brick->AddComponent<CTransform>(Vec2(96, 480));

	//// NOTE: Your final code should position the entity with the grid x,y position read from the file:
	//// brick->AddComponent<CTransform>(GridToMidPixel(gridX, gridY, brick));

	//if (brick->GetComponent<CAnimation>().animation.getName() == "Brick")
	//{
	//	std::cout << "This could be a good way of identifying if a tile is a brick!" << std::endl;
	//}

	//// NOTE: INCREDIBLY IMPORTANT
	////		 Components are now returned as references rather than pointers
	////		 If you don't specify a reference variable type it will COPY the component

	//// COPY EXAMPLE
	//// auto transform1 = entity->Get<CTranform>()

	//// REFERENCE EXAMPLE
	//// auto& transform2 = entity->get<CTransform>()
}

void Scene_Play::SpawnPlayer()
{
	// sample player
	m_player = m_entityManager.AddEntity("player");
	m_player->AddComponent<CAnimation>(m_game->assets().GetAnimation("Block"), true);
	m_player->AddComponent<CTransform>(GridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->AddComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
	m_player->AddComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->AddComponent<CInput>();

	// TODO: Add the remaining component to the player
}

void Scene_Play::SpawnBullet()
{
	// TODO: this should spawn a bullet at the given entity, going in the diretion the entity is facing
	std::shared_ptr<Entity> bullet = m_entityManager.AddEntity("bullet");
	Vec2 bulletVelocity = m_player->GetComponent<CTransform>().scale.x > 0 ? Vec2(5, 0) : Vec2(-5, 0);
	bullet->AddComponent<CTransform>(m_player->GetComponent<CTransform>().pos, bulletVelocity, Vec2(0.3f, 0.3f), 0);
	bullet->AddComponent<CAnimation>(m_game->assets().GetAnimation("QuestionBlock"), true);
	bullet->AddComponent<CBoundingBox>(Vec2(bullet->GetComponent<CAnimation>().animation.getSize().x, bullet->GetComponent<CAnimation>().animation.getSize().y));
	bullet->AddComponent<CLifeSpan>(300, m_currentFrame);
}

void Scene_Play::SpawnExplosion(const Vec2& spawnPos)
{
	std::shared_ptr<Entity> explosion = m_entityManager.AddEntity("explosion");
	explosion->AddComponent<CTransform>(spawnPos);
	explosion->GetComponent<CTransform>().scale = Vec2(2, 2);
	explosion->AddComponent<CAnimation>(m_game->assets().GetAnimation("Explosion"), false);
}

void Scene_Play::Update()
{
	m_entityManager.Update();

	// TODO: Implement pause functionality
	m_currentFrame++;
	SMovement();
	SLifespan();
	SCollision();
	SAnimation();
	SRenderer();
}

void Scene_Play::SMovement()
{
	Vec2 playerVelocity(m_player->GetComponent<CTransform>().velocity.x, m_player->GetComponent<CTransform>().velocity.y);

	if (m_player->GetComponent<CInput>().up && m_player->GetComponent<CState>().state == "grounded")
	{
		m_player->GetComponent<CState>().state = "jumping";
		playerVelocity.y = -m_playerConfig.JUMP;
	}
	
	if (m_player->GetComponent<CInput>().right)
	{
		playerVelocity.x += m_playerConfig.ACCELERATION;
		m_player->GetComponent<CTransform>().scale.x = 1;
	}
	else if (m_player->GetComponent<CInput>().left)
	{
		playerVelocity.x -= m_playerConfig.ACCELERATION;
		m_player->GetComponent<CTransform>().scale.x = -1;

	}
	else if (!m_player->GetComponent<CInput>().right && !m_player->GetComponent<CInput>().left)
	{
		if (playerVelocity.x != 0)
		{
			if (playerVelocity.x > 0)
			{
				playerVelocity.x = std::clamp(playerVelocity.x - m_playerConfig.ACCELERATION, 0.0f, m_playerConfig.MAXSPEED);
			}
			else if (playerVelocity.x < 0)
			{
				playerVelocity.x = std::clamp(playerVelocity.x + m_playerConfig.ACCELERATION, -m_playerConfig.MAXSPEED, 0.0f);
			}
		}
	}

	playerVelocity.x = std::clamp(playerVelocity.x, -m_playerConfig.MAXSPEED, m_playerConfig.MAXSPEED);
	m_player->GetComponent<CTransform>().velocity = playerVelocity;

	for (const auto& e : m_entityManager.GetEntities())
	{
		if (e->HasComponent<CGravity>())
		{
			e->GetComponent<CTransform>().velocity.y += e->GetComponent<CGravity>().gravity;
		}

		e->GetComponent<CTransform>().prevPos = e->GetComponent<CTransform>().pos;
		e->GetComponent<CTransform>().pos += e->GetComponent<CTransform>().velocity;
	}
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

	m_game->window().display();
}

void Scene_Play::SDebug()
{

}

void Scene_Play::SLifespan()
{
	// TODO: Check lifespan of entities that have them, and destroy them if they go over
	for (auto e : m_entityManager.GetEntities())
	{
		if (e->HasComponent<CLifeSpan>())
		{
			auto& lifespan = e->GetComponent<CLifeSpan>();

			if (m_currentFrame - lifespan.frameCreated >= lifespan.lifespan) e->destroy();
		}
	}
}

void Scene_Play::SCollision()
{
	// REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
	//			 This means jumping will have a negative y-component
	//			 and gravity will have a positve one

	// TODO: Implement Physics::GetOverlap() function here

	// TODO: Implement bullet/tile collisions
	//		 Destroy the tile if it has a Brick animation
	// Implement player/tile collisions and resolutions
	//		 Update the CState component of the plauer to store wether it is currently on the ground or in the air
	//		 This will be used by the animation system
	// Check to see if the plauer has fallen down a hole (y < height())
	// Don't let the player walk off the left side of the map

	// Player / Tile Resolution
	for (auto player : m_entityManager.GetEntities("player"))
	{
		auto& playerTransform = player->GetComponent<CTransform>();

		if (playerTransform.pos.x - player->GetComponent<CBoundingBox>().halfSize.x < 0) playerTransform.pos.x = player->GetComponent<CBoundingBox>().halfSize.x;

		if (playerTransform.pos.y > m_game->window().getSize().y + player->GetComponent<CBoundingBox>().halfSize.y)
		{
			playerTransform.pos = GridToMidPixel(m_playerConfig.X, m_playerConfig.Y, player);
			playerTransform.velocity *= 0;
		}

		for (auto tile : m_entityManager.GetEntities("tile"))
		{
			Vec2 overlap = Physics::GetOverlap(player, tile);
			if (overlap.x < 0 || overlap.y < 0) continue;
			
			Vec2 prevOverlap = Physics::GetPreviousOverlap(player, tile);
			Vec2 direction;
			float magnitude = 0;
			if (prevOverlap.x < 0 && prevOverlap.y < 0)
			{
				direction += Vec2(-playerTransform.velocity.x, 0);
				magnitude = overlap.x;
				playerTransform.velocity.x = 0;
			}
			else if (prevOverlap.x > 0)
			{
				direction += Vec2(0, -playerTransform.velocity.y);
				magnitude = overlap.y;
				if (playerTransform.velocity.y > 0)
				{
					m_player->GetComponent<CState>().state = "grounded";
				    SpawnExplosion(m_player->GetComponent<CTransform>().pos);
				}
				playerTransform.velocity.y = 0;
			}
			else if (prevOverlap.y > 0)
			{
				direction += Vec2(-playerTransform.velocity.x, 0);
				magnitude = overlap.x;
				playerTransform.velocity.x = 0;
			}
			playerTransform.pos += direction.Normalize() * magnitude;
		}
	}

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
		else if (action.Name() == "JUMP")
		{
			m_player->GetComponent<CInput>().up = true;
		}
		else if (action.Name() == "WALK-R")
		{
			m_player->GetComponent<CInput>().right = true;
		}
		else if (action.Name() == "WALK-L")
		{
			m_player->GetComponent<CInput>().left = true;
		}
		else if (action.Name() == "FIRE")
		{
			SpawnBullet();
		}
	}
	else if (action.Type() == "END")
	{
		if (action.Name() == "JUMP")
		{
			m_player->GetComponent<CInput>().up = false;
		}
		else if (action.Name() == "WALK-R")
		{
			m_player->GetComponent<CInput>().right = false;
		}
		else if (action.Name() == "WALK-L")
		{
			m_player->GetComponent<CInput>().left = false;
		}
	}
}

void Scene_Play::SAnimation()
{
	// TODO: Complete the Animation class code first

	//if (m_player->GetComponent<CState>().state == "jumping")
	//{
	//	m_player->AddComponent<CAnimation>(m_game->assets().GetAnimation("Air"), true);
	//}

	// TODO: set the animation of the player based on its CState component
	// TODO: for each entity with an animation, call entity->GetComponent<CAnimation>().animation.Update()
	//		 if the animation is not repeated and it has ended, destroy the entity
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CAnimation>()) continue;

		e->GetComponent<CAnimation>().animation.update();
		if (e->GetComponent<CAnimation>().animation.hasEnded() && !e->GetComponent<CAnimation>().repeat) e->destroy();
	}
}

void Scene_Play::OnEnd()
{
	// TODO: When the scene ends, change bach to the MENU scene
	m_game->window().close();
}
