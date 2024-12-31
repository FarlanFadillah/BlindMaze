#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "Scene.h"

class Scene;

struct WindowConfig {
	int W, H, FL, FS;
	std::string T;
};

class Engine
{
	sf::RenderWindow m_window;
	uint64_t currentFrame = 0;
	std::map<std::string, std::shared_ptr<Scene>> m_sceneMap;

	std::string currentSceneName = "NONE";
	bool running = true;
	WindowConfig winConf;

	void update();
	void sUserInput();
public:

	Engine();
	Engine(const std::string& config);

	void init(const std::string& config);

	void changeScene(const std::string& sceneName, const std::shared_ptr<Scene> scene);
	const std::shared_ptr<Scene> currentScene();
	void run();
	void quit();
	bool isRunning();

	sf::RenderWindow& window();

};

