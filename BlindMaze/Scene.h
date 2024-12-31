#pragma once
#include "Engine.h"
#include "Action.h"
#include <string>
#include <map>

class Engine;

using ActionMap = std::map<int, std::string>;

class Scene
{
protected:
	Engine*		m_game			= nullptr;
	bool		m_paused		= false;
	bool		m_hasEnded		= false;
	uint64_t	currentFrame	= 0;
	ActionMap	m_actionMap;
public:
	Scene();
	Scene(Engine* engine);

	virtual void update()		= 0;
	virtual void sRender()		= 0;
	virtual void sDoAction(const Action& action)	= 0;
	virtual void onEnd()		= 0;


	const ActionMap& getActionMap() const;
	Engine* gameEngine();
	void registerAction(int inputKey, const std::string& actionName);
	void setPaused(bool paused);

};

