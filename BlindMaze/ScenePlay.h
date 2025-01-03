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
	bool m_playerLight		= true;
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
	void updateRay(std::shared_ptr<Entity> e);
	void setRayForPlayer(std::shared_ptr<Entity> e);
	void setRayForTorch(std::shared_ptr<Entity> e);
	void readMap();
	void spawnTorch(const int col, const int row, const int s, const bool init);

	void testMap();

public:

	ScenePlay();
	ScenePlay(Engine* engine);

};

