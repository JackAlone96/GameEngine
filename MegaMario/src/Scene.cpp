#include "Scene.h"

void Scene::SetPaused(bool paused)
{
	m_paused = paused;
}

Scene::Scene() {}

Scene::Scene(GameEngine* gameEngine)
	: m_game(gameEngine) {}

void Scene::DoAction(const Action& action)
{

}

void Scene::Simulate(const size_t frames)
{
	for (int i = 0; i < frames; i++)
	{
		Update();
	}
}

void Scene::RegisterAction(int inputKey, const std::string& actionName)
{
	m_actionMap[inputKey] = actionName;
}

size_t Scene::Width() const
{
	return size_t(); // TODO: ???
}

size_t Scene::Height() const
{
	return size_t(); // TODO: ???
}

size_t Scene::CurrentFrame() const
{
	return m_currentFrame;
}

bool Scene::HasEnded() const
{
	return m_hasEnded;
}

const ActionMap& Scene::GetActionMap() const
{
	return m_actionMap;
}

void Scene::DrawLine(const Vec2& p1, const Vec2& p2)
{
	// TODO: How?
}
