#include "Scene.h"

Scene::Scene()
{
}

Scene::Scene(Engine* engine)
	:m_game(engine)
{
}

const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

void Scene::registerAction(const int key, const std::string& name)
{
	m_actionMap[key] = name;
}

Engine* Scene::gameEngine()
{
	return m_game;
}


void Scene::setPaused(bool paused)
{
	m_paused = true;
}
