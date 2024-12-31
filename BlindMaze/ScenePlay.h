#pragma once
#include "Scene.h"
#include "EntityManager.hpp"
class ScenePlay : public Scene
{
private:
	EntityManager m_entityManager;
	void update()							override;
	void sRender()							override;
	void sDoAction(const Action& action)	override;
	void onEnd()							override; 
public:

	ScenePlay();
	ScenePlay(Engine* engine);

};

