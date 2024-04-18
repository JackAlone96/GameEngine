#include "GameEngine.h"

void GameEngine::init(const std::string&& path)
{
	// TODO
}

void GameEngine::update()
{
	// TODO
}

void GameEngine::sUserInput()
{
	// TODO
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	// TODO:
}

GameEngine::GameEngine(const std::string& path)
{
	// TODO
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	// TODO
}

void GameEngine::quit()
{
	m_running = false;
}

void GameEngine::run()
{
	m_running = true;
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

const Assets& GameEngine::assets() const
{
	return m_assets;
}

bool GameEngine::isRunning()
{
	return m_running;
}
