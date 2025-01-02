#pragma once
#include "Scene.h"
#include "EntityManager.hpp"
#include "Entity.hpp"
#include "Physics.h"

class ScenePlay : public Scene
{
private:
	EntityManager m_entityManager;
	Physics phy;
	std::shared_ptr<Entity> player;

	/*std::vector<sf::Vertex*> ray;
	std::vector<sf::Vertex*> light;
	std::vector<sf::Vertex> angle;*/

	bool m_drawEntities		= true;
	bool m_drawMousePos		= false;
	bool m_drawLight		= true;
	bool m_drawRay			= false;

	bool m_transParentBox	= true;
	bool m_collision		= true;

	sf::Vector2f m_pos;

	sf::VertexArray visibilityPolygon; 
	void update()							override;
	void sRender()							override;
	void sDoAction(const Action& action)	override;
	void sMovement();
	void onEnd()							override; 
	void sCollision();

	void sRayCasting();
	void readMap();
	
public:

	ScenePlay();
	ScenePlay(Engine* engine);

};

