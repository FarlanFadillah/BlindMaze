#pragma once
#include "Scene.h"
#include "EntityManager.hpp"
#include "Entity.hpp"
#include "Physics.h"

class ScenePlay : public Scene
{
private:

	std::vector<std::string> levels;

	EntityManager m_entityManager;
	Physics phy;
	std::shared_ptr<Entity> player;

	sf::Font ostrich_regular;

	/*std::vector<sf::Vertex*> ray;
	std::vector<sf::Vertex*> light;
	std::vector<sf::Vertex> angle;*/

	size_t m_numDoor		= 0;
	int level = 0;

	bool debugMode;

	bool m_drawEntities;
	bool m_drawPlayer;
	bool m_drawMousePos;
	bool m_drawLight;
	bool m_playerLight;
	bool m_drawRay;
	bool m_drawGui;

	bool m_transParentBox;
	bool m_collision;

	bool mouseMoved			= false;

	sf::View CameraView;

	sf::Vector2f m_pos;
	sf::Vector2f m_wpos;

	sf::VertexArray visibilityPolygon; 
	void update()							override;
	void sRender()							override;
	void sDoAction(const Action& action)	override;
	void sMovement();
	void onEnd()							override; 
	void sCollision();
	void sCamera();
	void objective();

	void sRayCasting();
	void updateRay(std::shared_ptr<Entity> e);
	void setRayForPlayer(std::shared_ptr<Entity> e);
	void setRayForTorch(std::shared_ptr<Entity> e);
	void updateTorchRay(std::shared_ptr<Entity> e);


	void readMap(const std::string& level);
	void imageToMap(const std::string& fileName);

	void wallCollision(const std::string& tag);
	void itemCollision(const std::string& tag);
	void doorCollision(const sf::Vector2f& pos, const std::string& tag);

	void spawnPlayer(const int col, const int row, const int s);
	void spawnWall(const int col, const int row, const int s, bool transparent);
	void spawnDoor(const int col, const int row, const int s);
	void spawnTorch(const int col, const int row, const int s, const bool init);
	void spawnKey(const int col, const int row, const int s);


	void initLevel(const std::string& config);

	void testMap();

	sf::Vector2f windowToWorldPos(const sf::Vector2f& pos);

public:

	ScenePlay();
	ScenePlay(Engine* engine, const int level, const std::string& config);

	void init(const std::string& config);

};

