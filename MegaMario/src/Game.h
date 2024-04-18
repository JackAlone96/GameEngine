#pragma once

#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow m_window; // the window we will draw to
	sf::Clock m_deltaClock;    // Clock used by ImGui
	EntityManager m_entities;  // vector of the entities to mantain
	sf::Font m_font;
	sf::Text m_text;
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;

	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnFrame = 0;
	bool m_paused = false;
	bool m_running = true;

	bool m_isLifespanOn = true;
	bool m_isEnemySpawnerOn = true;
	bool m_isMovementOn = true;
	bool m_isCollisionOn = true;

	std::shared_ptr<Entity> m_player;

	void Init(const std::string& config); // Initialize the GameState with a config file
	void SetPaused(bool paused);          // pause the game

	// Systems
	void SMovement();                     // System: Entity position / movement update
	void SUserInput();					  // System: User Input
	void SLifespan();					  // System: Lifespan
	void SRenderer();					  // System: Render / Drawing
	void SEnemySpawner();	              // System: Spawn Enemies
	void SBulletSpawner();
	void SCollision();		              // System: Collisions
	void SGui();
	
	void SpawnPlayer();
	void SpawnEnemy();
	void SpawnSmallEnemy(std::shared_ptr<Entity> e);
	void SpawnBullet(std::shared_ptr<Entity> entity, const Math::Vec2& mousePos);
	void SpawnSpecialWeapon(std::shared_ptr<Entity> entity);

	

public:
	void Run();
	Game(const std::string& path);

};