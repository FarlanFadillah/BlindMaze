#include "ScenePlay.h"



ScenePlay::ScenePlay()
{
}

ScenePlay::ScenePlay(Engine* engine)
	:Scene(engine)
{
}

void ScenePlay::update()
{
	if (!m_paused)
	{
		m_entityManager.update();

	}

	sRender();
}

void ScenePlay::sRender()
{
	auto& window = m_game->window();
	window.clear(sf::Color::Black);
}

void ScenePlay::sDoAction(const Action& action)
{
}

void ScenePlay::onEnd()
{
}
