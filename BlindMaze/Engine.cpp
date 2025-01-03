#include "Engine.h"
#include "ScenePlay.h"
#include <fstream>
#include <iostream>

Engine::Engine()
{
}

Engine::Engine(const std::string& config)
{
	init(config);
}

void Engine::init(const std::string& config)
{
	std::fstream fin(config);

	while (fin.good())
	{
		std::string token;
		fin >> token >> winConf.W >> winConf.H >> winConf.FL >> winConf.FS >> winConf.T;
	}

	if (winConf.FS)
	{
		m_window.create(sf::VideoMode(winConf.W, winConf.H, sf::Style::Fullscreen), winConf.T);
	}
	else
	{
		m_window.create(sf::VideoMode(winConf.W, winConf.H, sf::Style::Close), winConf.T);
	}

	m_window.setFramerateLimit(winConf.FL);

	changeScene("PLAY", std::make_shared<ScenePlay>(this));
}

void Engine::update()
{
	sUserInput();
	currentScene()->update();
	m_window.display();
}

void Engine::sUserInput()
{

	sf::Event event;

	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		else if (event.type == sf::Event::KeyPressed || 
				event.type == sf::Event::KeyReleased)
		{

			if (currentScene()->getActionMap().find(event.key.code) ==
				currentScene()->getActionMap().end())
			{
				continue;
			}

			std::string actionType = 
				(event.type == sf::Event::KeyPressed) ? "START" : "END";

			currentScene()->sDoAction(Action(
				actionType,
				currentScene()->getActionMap().at(event.key.code)
			));

		}
		else if (event.type == sf::Event::MouseMoved)
		{
			currentScene()->sDoAction(Action(sf::Vector2f(event.mouseMove.x, event.mouseMove.y)));
		}
		else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
		{
			std::string actionType =
				(event.type == sf::Event::MouseButtonPressed) ? "START" : "END";
			currentScene()->sDoAction(Action(sf::Vector2f(event.mouseButton.x, event.mouseButton.y), actionType, "MOUSE_CLICKED"));
		}

	}
}

void Engine::changeScene(const std::string& sceneName, const std::shared_ptr<Scene> scene)
{
	currentSceneName = sceneName;
	m_sceneMap[currentSceneName] = scene;
}

const std::shared_ptr<Scene> Engine::currentScene()
{
	return m_sceneMap.at(currentSceneName);
}

void Engine::run()
{
	while (running && m_window.isOpen())
	{
		update();
	}

	quit();
}

void Engine::quit()
{
	running = false;
	m_window.close();
}

bool Engine::isRunning()
{
	return running && m_window.isOpen();
}

sf::RenderWindow& Engine::window()
{
	return m_window;
}





