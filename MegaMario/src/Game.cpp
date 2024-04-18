#include "Game.h"
#include <fstream>
#include <filesystem>
#define _USE_MATH_DEFINES
#include <math.h>

Game::Game(const std::string& config)
{
	Init(config);
}

void Game::Init(const std::string& config)
{
	std::ifstream fin(config);
	std::string label;

	// window
	int wWidth = 0;
	int wHeight = 0;
	int fl = 0;
	bool fullscreen = true;

	// font
	std::string fontPath;
	int fontSize = 0;
	int fontColorR = 0;
	int fontColorG = 0;
	int fontColorB = 0;

	// READ FROM THE CONFIGURATION FILE
	while (fin >> label)
	{
		if (label == "Window")
		{
			fin >> wWidth >> wHeight >> fl >> fullscreen;
		}
		else if (label == "Font")
		{
			fin >> fontPath >> fontSize >> fontColorR >> fontColorG >> fontColorB;
		}
		else if (label == "Player")
		{
			fin >> m_playerConfig.SR;
			fin >> m_playerConfig.CR;
			fin >> m_playerConfig.S;
			fin >> m_playerConfig.FR;
			fin >> m_playerConfig.FG;
			fin >> m_playerConfig.FB;
			fin >> m_playerConfig.OR;
			fin >> m_playerConfig.OG;
			fin >> m_playerConfig.OB;
			fin >> m_playerConfig.OT;
			fin >> m_playerConfig.V;
		}
		else if (label == "Enemy")
		{
			fin >> m_enemyConfig.SR;
			fin >> m_enemyConfig.CR;
			fin >> m_enemyConfig.SMIN;
			fin >> m_enemyConfig.SMAX;
			fin >> m_enemyConfig.OR;
			fin >> m_enemyConfig.OG;
			fin >> m_enemyConfig.OB;
			fin >> m_enemyConfig.OT;
			fin >> m_enemyConfig.VMIN;
			fin >> m_enemyConfig.VMAX;
			fin >> m_enemyConfig.L;
			fin >> m_enemyConfig.SI;

		}
		else if (label == "Bullet")
		{
			fin >> m_bulletConfig.SR;
			fin >> m_bulletConfig.CR;
			fin >> m_bulletConfig.S;
			fin >> m_bulletConfig.FR;
			fin >> m_bulletConfig.FG;
			fin >> m_bulletConfig.FB;
			fin >> m_bulletConfig.OR;
			fin >> m_bulletConfig.OG;
			fin >> m_bulletConfig.OB;
			fin >> m_bulletConfig.OT;
			fin >> m_bulletConfig.V;
			fin >> m_bulletConfig.L;
		}
	}


	// set up default window parameters
	m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2", fullscreen == true ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.setFramerateLimit(fl);

	// initialize ImGui
	ImGui::SFML::Init(m_window);
	ImGui::GetStyle().ScaleAllSizes(1.0f);

	if (!m_font.loadFromFile((std::filesystem::current_path() / fontPath).string()))
	{
		// if we can't load the font, print an error to the error console and exit
		std::cerr << "Could not load font!" << std::endl;
		exit(-1);
	}

	m_text = sf::Text("Score: ", m_font, fontSize);
	m_text.setFillColor(sf::Color(fontColorR, fontColorG, fontColorB));
	m_text.setOutlineColor(sf::Color(fontColorR, fontColorG, fontColorB));
	m_text.setPosition(0, 0);

	SpawnPlayer();

	srand(time(NULL));
}

void Game::Run()
{
	while (m_running)
	{
		m_entities.Update();

		SUserInput();
		
		if (!m_paused)
		{
			SEnemySpawner();
			SBulletSpawner();
			SMovement();
			SCollision();
			SLifespan();
		}
		
		SGui();
		SRenderer();

		// increment the current frame
		// may need to be moved when pause implemented
		if (!m_paused) m_currentFrame++;
	}
}

void Game::SetPaused(bool paused)
{
	m_paused = paused;
}

// spawn/respawn the player in the middle of the screen
void Game::SpawnPlayer()
{
	// We create every entity by calling EntityManager.addEntity(tag)
	// This returns a std::shared_ptr<Entity>, we use auto to save typing
	auto entity = m_entities.AddEntity("player");

	// Get the middle of the screen
	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	entity->cTransform = std::make_shared<CTransform>(Math::Vec2(mx, my), Math::Vec2(0.0f, 0.0f), 0.0f);

	// The entity's shape with values from the configuration file
	sf::Color fillColor = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
	sf::Color outlineColor = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);
	entity->cCircleShape = std::make_shared<CCircleShape>((float)m_playerConfig.SR, m_playerConfig.V, fillColor, outlineColor, m_playerConfig.OT);

	// Add an input component to the player so that we can use inputs
	entity->cInput = std::make_shared<CInput>();

	// Add a collision component
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	m_player = entity;
}

// spawn an enemy at a random position
void Game::SpawnEnemy()
{
	auto entity = m_entities.AddEntity("enemy");

	float ex = rand() % (m_window.getSize().x - (m_enemyConfig.CR * 2) + 1) + m_enemyConfig.CR;
	float ey = rand() % (m_window.getSize().y - (m_enemyConfig.CR * 2) + 1) + m_enemyConfig.CR;

	float randomSpeed = rand() % (int)(m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1) + m_enemyConfig.SMIN;
	float randomAngle = rand() % 361;
	float vx = cosf(randomAngle * (M_PI / 180));
	float vy = -sinf(randomAngle * (M_PI / 180));

	entity->cTransform = std::make_shared<CTransform>(Math::Vec2(ex, ey), Math::Vec2(vx * randomSpeed, vy * randomSpeed), 0.0f);

	int randomVertices = rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1) + m_enemyConfig.VMIN;
	int randomFillColorR = rand() % 256;
	int randomFillColorG = rand() % 256;
	int randomFillColorB = rand() % 256;

	sf::Color fillColor = sf::Color(randomFillColorR, randomFillColorG, randomFillColorB);
	sf::Color outlineColor = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

	entity->cCircleShape = std::make_shared<CCircleShape>(m_enemyConfig.SR, randomVertices, fillColor, outlineColor, m_enemyConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
	
	entity->cScore = std::make_shared<CScore>(entity->cCircleShape->points * 100.0f);
}

// spawns the small enemies when a big one (input entity) explodes
void Game::SpawnSmallEnemy(std::shared_ptr<Entity> e)
{
	for (int i = 0; i < e->cCircleShape->points; i ++)
	{
		auto entity = m_entities.AddEntity("small");

		float angleOfSpawn = (360 / e->cCircleShape->points) * i;
		float vx = cosf((angleOfSpawn * (M_PI / 180)));
		float vy = sinf((angleOfSpawn * (M_PI / 180)));
		
		Math::Vec2 velocity = Math::Vec2(vx * 3.0f, vy * 3.0f);
		entity->cTransform = std::make_shared<CTransform>(e->cTransform->position, velocity, 0.0f);

		entity->cCircleShape = std::make_shared<CCircleShape>(e->cCircleShape->radius / 2, e->cCircleShape->points, e->cCircleShape->circle.getFillColor(), e->cCircleShape->circle.getOutlineColor(), e->cCircleShape->circle.getOutlineThickness());

		entity->cScore = std::make_shared<CScore>(e->cScore->score * 2);

		entity->cCollision = std::make_shared<CCollision>(e->cCollision->radius / 2);

		entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
	}
}

// spawns a bullet from a given entity to a target location
void Game::SpawnBullet(std::shared_ptr<Entity> entity, const Math::Vec2& mousePos)
{
	Math::Vec2 startPosition = entity->cTransform->position;
	Math::Vec2 speed = (mousePos - startPosition).Normalize() * m_bulletConfig.S;

	auto b = m_entities.AddEntity("bullet");

	b->cTransform = std::make_shared<CTransform>(startPosition + (mousePos - startPosition).Normalize() * entity->cCollision->radius, speed, 0);

	sf::Color fillColor = sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
	sf::Color outlineColor = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);

	b->cCircleShape = std::make_shared<CCircleShape>(m_bulletConfig.SR, m_bulletConfig.V, fillColor, outlineColor, m_bulletConfig.OT);

	b->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	b->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::SpawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO: spawn your own special weapon
}

void Game::SMovement()
{
	if (!m_isMovementOn) return;

	if (m_player->cTransform)
	{
		// reset player velocity each frame
		m_player->cTransform->velocity = { 0,0 };
		
		// implement player velocity
		if (m_player->cInput->up)
		{
			m_player->cTransform->velocity.y = -1;
		}

		if (m_player->cInput->left)
		{
			m_player->cTransform->velocity.x = -1;
		}

		if (m_player->cInput->down)
		{
			m_player->cTransform->velocity.y = 1;
		}

		if (m_player->cInput->right)
		{
			m_player->cTransform->velocity.x = 1;
		}

		m_player->cTransform->velocity = m_player->cTransform->velocity.Normalize() * m_playerConfig.S;
	}

	// move all entities
	for (auto e : m_entities.GetEntities())
	{
		if (e->cTransform)
		{
			e->cTransform->position += e->cTransform->velocity;
		}
	}
}

void Game::SLifespan()
{
	if (!m_isLifespanOn) return;

	for (auto e : m_entities.GetEntities())
	{
		if (!e->cLifespan) continue;

		if (e->cLifespan->remaining > 0)
		{
			e->cLifespan->remaining--;
			sf::Color color = e->cCircleShape->circle.getFillColor();
			sf::Color outColor = e->cCircleShape->circle.getOutlineColor();
			color.a = ((float)e->cLifespan->remaining / e->cLifespan->total) * 255;
			outColor.a = ((float)e->cLifespan->remaining / e->cLifespan->total) * 255;
			e->cCircleShape->circle.setFillColor(color);
			e->cCircleShape->circle.setOutlineColor(outColor);
		}
		else
		{
			e->destroy();
		}
	}
}

void Game::SRenderer()
{
	m_window.clear();

	// draw all the entity's shapes
	for (auto e : m_entities.GetEntities())
	{
		// set the position of the shape based on the entity's transform->pos
		e->cCircleShape->circle.setPosition(e->cTransform->position.x, e->cTransform->position.y);

		// set the rotation of the shape based on the entity's transform->angle
		e->cTransform->angle += 1.0f;
		e->cCircleShape->circle.setRotation(e->cTransform->angle);

		// draw the entity's sf::CircleShape
		m_window.draw(e->cCircleShape->circle);
	}

	std::string scoreText = "Score: " + std::to_string(m_score);
	
	m_text.setString(scoreText);
	m_window.draw(m_text);

	ImGui::SFML::Render(m_window); // Draw the GUI for last so it's on top

	m_window.display();
}

void Game::SEnemySpawner()
{
	if (!m_isEnemySpawnerOn) return;

	if (m_currentFrame - m_lastEnemySpawnFrame >= m_enemyConfig.SI)
	{
		SpawnEnemy();
		
		m_lastEnemySpawnFrame = m_currentFrame;
	}
}

void Game::SBulletSpawner()
{
	if (m_player->cInput->shoot)
	{
		SpawnBullet(m_player, m_player->cInput->mousePosition);
	}
}

void Game::SCollision()
{
	if (!m_isCollisionOn) return;

	for (auto bullet : m_entities.GetEntities("bullet"))
	{
		for (auto enemy : m_entities.GetEntities("enemy"))
		{
			if (bullet->cTransform->position.Distance(enemy->cTransform->position) <= bullet->cCollision->radius + enemy->cCollision->radius)
			{
				m_score += enemy->cScore->score;
				bullet->destroy();
				SpawnSmallEnemy(enemy);
				enemy->destroy();
			}
		}

		for (auto smallEnemy : m_entities.GetEntities("small"))
		{
			if (bullet->cTransform->position.Distance(smallEnemy->cTransform->position) <= bullet->cCollision->radius + smallEnemy->cCollision->radius)
			{
				m_score += smallEnemy->cScore->score;
				bullet->destroy();
				smallEnemy->destroy();
			}
		}
	}

	for (auto enemy : m_entities.GetEntities("enemy"))
	{
		if (m_player->cTransform->position.Distance(enemy->cTransform->position) <= m_player->cCollision->radius + enemy->cCollision->radius)
		{
			m_player->destroy();
			SpawnSmallEnemy(enemy);
			enemy->destroy();
			SpawnPlayer();
		}

		if (enemy->cTransform->position.x + enemy->cCollision->radius >= m_window.getSize().x && enemy->cTransform->velocity.x > 0 || enemy->cTransform->position.x - enemy->cCollision->radius <= 0 && enemy->cTransform->velocity.x < 0)
		{
			enemy->cTransform->velocity.x *=  -1;
		}
		else if (enemy->cTransform->position.y + enemy->cCollision->radius >= m_window.getSize().y && enemy->cTransform->velocity.y > 0 || enemy->cTransform->position.y - enemy->cCollision->radius <= 0 && enemy->cTransform->velocity.y < 0)
		{
			enemy->cTransform->velocity.y *= -1;
		}
	}

	for (auto smallEnemy : m_entities.GetEntities("small"))
	{
		if (m_player->cTransform->position.Distance(smallEnemy->cTransform->position) <= m_player->cCollision->radius + smallEnemy->cCollision->radius)
		{
			smallEnemy->destroy();
		}
	}

	if (m_player->cTransform->position.x + m_player->cCollision->radius >= m_window.getSize().x && m_player->cTransform->velocity.x > 0)
	{
		m_player->cTransform->position.x = m_window.getSize().x - m_player->cCollision->radius;
	}
	else if (m_player->cTransform->position.x - m_player->cCollision->radius <= 0 && m_player->cTransform->velocity.x < 0)
	{
		m_player->cTransform->position.x = m_player->cCollision->radius;
	}

	if (m_player->cTransform->position.y + m_player->cCollision->radius >= m_window.getSize().y && m_player->cTransform->velocity.y > 0)
	{
		m_player->cTransform->position.y = m_window.getSize().y - m_player->cCollision->radius;
	}
	else if (m_player->cTransform->position.y - m_player->cCollision->radius <= 0 && m_player->cTransform->velocity.y < 0)
	{
		m_player->cTransform->position.y = m_player->cCollision->radius;
	}
}

void Game::SUserInput()
{
	// Reset input
	if (m_player->cInput->shoot) m_player->cInput->shoot = false;

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// this event triggers when the window is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// this event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W Key Pressed" << std::endl;
				m_player->cInput->up = true;
				break;

			case sf::Keyboard::A:
				std::cout << "A Key Pressed" << std::endl;
				m_player->cInput->left = true;
				break;

			case sf::Keyboard::S:
				std::cout << "S Key Pressed" << std::endl;
				m_player->cInput->down = true;
				break;

			case sf::Keyboard::D:
				std::cout << "D Key Pressed" << std::endl;
				m_player->cInput->right = true;
				break;
			
			case sf::Keyboard::Escape:
				m_window.close();
				break;

			case sf::Keyboard::P:
				SetPaused(!m_paused);
				break;
			}
		}

		// this event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W Key Released" << std::endl;
				m_player->cInput->up = false;
				break;

			case sf::Keyboard::A:
				std::cout << "A Key Released" << std::endl;
				m_player->cInput->left = false;
				break;

			case sf::Keyboard::S:
				std::cout << "S Key Released" << std::endl;
				m_player->cInput->down = false;
				break;

			case sf::Keyboard::D:
				std::cout << "D Key Released" << std::endl;
				m_player->cInput->right = false;
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::cout << "Left mouse button clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
				m_player->cInput->shoot = true;
				m_player->cInput->mousePosition = Math::Vec2(event.mouseButton.x, event.mouseButton.y);
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				std::cout << "Right mouse button clicked at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
				// call spawnSpecialWeapon here
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_player->cInput->shoot = false;
			}
		}

		// Process events for ImGui
		ImGui::SFML::ProcessEvent(m_window, event);
	}
}

void Game::SGui()
{
	// update imgui for this frame with the time the last frame took
	ImGui::SFML::Update(m_window, m_deltaClock.restart());

	// build and draw the UI
	ImGui::Begin("Geometry wars settings");
	ImGui::ShowDemoWindow();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Systems"))
		{
			ImGui::Checkbox("Movement", &m_isMovementOn);
			ImGui::Checkbox("Lifespan", &m_isLifespanOn);
			ImGui::Checkbox("Collision", &m_isCollisionOn);
			ImGui::Checkbox("Spawning", &m_isEnemySpawnerOn);
			ImGui::Indent();
			ImGui::SliderInt("Spawn Interval", &m_enemyConfig.SI, 0, 600);
			if (ImGui::Button("Spawn enemy")) SpawnEnemy();
			ImGui::Unindent();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Entity Manager"))
		{
			if (ImGui::CollapsingHeader("Entities by tag"))
			{
				ImGui::Indent();
				for (auto entityTag : m_entities.GetEntityMap())
				{
					if (ImGui::CollapsingHeader(entityTag.first.c_str()))
					{
						ImGui::Indent();
						for (auto entity : entityTag.second)
						{
							ImVec4 color = ImColor(entity->cCircleShape->circle.getFillColor().r, entity->cCircleShape->circle.getFillColor().g, entity->cCircleShape->circle.getFillColor().b, entity->cCircleShape->circle.getFillColor().a);
							std::string name = "D##" + std::to_string(entity->id());
							ImGui::PushStyleColor(ImGuiCol_Button, color);
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
							if (ImGui::Button(name.c_str())) entity->destroy();
							ImGui::SameLine();
							ImGui::TextUnformatted(std::to_string(entity->id()).c_str());
							ImGui::SameLine();
							ImGui::TextUnformatted((entity->tag()).c_str());
							ImGui::SameLine();
							ImGui::TextUnformatted(entity->cTransform->position.ToString().c_str());
						}
						ImGui::Unindent();
					}
				}
				ImGui::Unindent();
			}

			if (ImGui::CollapsingHeader("All entities"))
			{
				ImGui::Indent();
				for (auto entity : m_entities.GetEntities())
				{
					ImVec4 color = ImColor(entity->cCircleShape->circle.getFillColor().r, entity->cCircleShape->circle.getFillColor().g, entity->cCircleShape->circle.getFillColor().b, entity->cCircleShape->circle.getFillColor().a);
					std::string name = "D##" + std::to_string(entity->id());
					ImGui::PushStyleColor(ImGuiCol_Button, color);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
					if (ImGui::Button(name.c_str())) entity->destroy();
					ImGui::SameLine();
					ImGui::TextUnformatted(std::to_string(entity->id()).c_str());
					ImGui::SameLine();
					ImGui::TextUnformatted((entity->tag()).c_str());
					ImGui::SameLine();
					ImGui::TextUnformatted(entity->cTransform->position.ToString().c_str());
				}
				ImGui::Unindent();

			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}


	ImGui::End();
}


