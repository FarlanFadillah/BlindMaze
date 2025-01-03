#include "ScenePlay.h"
#include <fstream>
#include <json/json.h>


ScenePlay::ScenePlay()
{
}

ScenePlay::ScenePlay(Engine* engine)
	:Scene(engine)
{

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::F1, "DRAW_LIGHT");
	registerAction(sf::Keyboard::F2, "DRAW_RAY");
	registerAction(sf::Keyboard::F3, "DRAW_ENTITIES");
	registerAction(sf::Keyboard::F4, "DRAW_MOUSEPOS");
	registerAction(sf::Keyboard::F5, "TRANSPARENT_BOX");
	registerAction(sf::Keyboard::F6, "COLLISION");
	registerAction(sf::Keyboard::F, "PLAYER_LIGHT");


	player = m_entityManager.addEntity("player");

	player->addComponent<CTransform>(Vec2(300, 300), Vec2(5, 5));
	player->addComponent<CBoundingBox>(Vec2(16, 16));
	player->addComponent<CVertex>(Vec2(300, 300), Vec2(16, 16));
	player->addComponent<CInput>();
	player->addComponent<CLight>(120, 250);
	m_entityManager.addEntity(player, "vertex_object");
	readMap();
	//testMap();
	
	

	/*auto& window = m_game->window();
	auto winSize = window.getSize();

	auto winFrame = m_entityManager.addEntity("wall");
	winFrame->addComponent<CVertex>(Vec2(winSize.x / 2, winSize.y / 2), Vec2(winSize.x, winSize.y));*/
}



void ScenePlay::update()
{
	if (!m_paused)
	{
		m_entityManager.update();
		sMovement();
		if(m_collision) sCollision();
		sRayCasting();
	}

	sRender();
}

void ScenePlay::sRender()
{
	auto& window = m_game->window();
	window.clear(sf::Color::Black);

	//draw player light
	auto& pl = player->getComponent<CLight>();
	if (m_drawLight && m_playerLight)
	{
		window.draw(pl.light);
	}
	if (m_drawRay)
	{
		for (auto& e : pl.ray)
		{
			window.draw(e, 2, sf::Lines);
		}
	}

	//draw all entities light
	for (auto& e : m_entityManager.getEntities("torch"))
	{
		if (e->hasComponent<CLight>())
		{
			auto& pl = e->getComponent<CLight>();
			if (m_drawLight)
			{
				window.draw(pl.light);
			}
			if (m_drawRay)
			{
				for (auto& e : pl.ray)
				{
					window.draw(e, 2, sf::Lines);
				}
			}
		}
	}

	//draw all entities
	if (m_drawEntities)
	{
		for (auto& e : m_entityManager.getEntities())
		{
			auto& ct = e->getComponent<CTransform>();
			auto& bb = e->getComponent<CBoundingBox>();

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(bb.size.x, bb.size.y));
			rect.setPosition(ct.pos.x, ct.pos.y);
			rect.setOrigin(bb.size.x / 2, bb.size.y / 2);
			rect.setFillColor((e == player) ? sf::Color::White : m_transParentBox ? sf::Color(0, 255, 0, 0) : sf::Color(0, 255, 0, 255));

			window.draw(rect);
		}
	}


	if (m_drawMousePos)
	{
		sf::CircleShape m(8, 30);
		m.setFillColor(sf::Color::Red);
		m.setOrigin(sf::Vector2f(4, 4));
		m.setPosition(m_pos);
		window.draw(m);
	}
	
}

void ScenePlay::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			player->getComponent<CInput>().up = true;
		}
		else if (action.name() == "DOWN")
		{
			player->getComponent<CInput>().down = true;
		}
		else if (action.name() == "LEFT")
		{
			player->getComponent<CInput>().left = true;
		}
		else if (action.name() == "RIGHT")
		{
			player->getComponent<CInput>().right = true;
		}
		else if (action.name() == "MOUSE_MOVED")
		{
			m_pos = action.pos();
		}
		else if (action.name() == "PLAYER_LIGHT")
		{
			m_playerLight = !m_playerLight;
		}
		else if (action.name() == "MOUSE_CLICKED")
		{
			//std::cout << action.pos().x << " " << action.pos().y << std::endl;
			auto& grid = phy.getRectGrid(Vec2(action.pos().x, action.pos().y), 32, false);
			spawnTorch(grid.x, grid.y, 32, false);
		}
	}
	else if (action.type() == "END")
	{
		if (action.name() == "UP")
		{
			player->getComponent<CInput>().up = false;
		}
		else if (action.name() == "DOWN")
		{
			player->getComponent<CInput>().down = false;
		}
		else if (action.name() == "LEFT")
		{
			player->getComponent<CInput>().left = false;
		}
		else if (action.name() == "RIGHT")
		{
			player->getComponent<CInput>().right = false;
		}
		else if (action.name() == "DRAW_LIGHT")
		{
			std::cout << "ss\n";
			m_drawLight = !m_drawLight;
		}
		else if (action.name() == "DRAW_RAY")
		{
			m_drawRay = !m_drawRay;
		}
		else if (action.name() == "DRAW_ENTITIES")
		{
			m_drawEntities = !m_drawEntities;
		}
		else if (action.name() == "DRAW_MOUSEPOS")
		{
			m_drawMousePos = !m_drawMousePos;
		}
		else if (action.name() == "TRANSPARENT_BOX")
		{
			m_transParentBox = !m_transParentBox;
		}else if (action.name() == "COLLISION")
		{
			m_collision = !m_collision;
		}
		else if (action.name() == "MOUSE_CLICKED")
		{
			//std::cout << action.pos().x << " " << action.pos().y << std::endl;

		}
	}
}

void ScenePlay::sMovement()
{
	Vec2 playerVelocity(0, 0);

	auto& input = player->getComponent<CInput>();
	auto& playerTransform = player->getComponent<CTransform>();
	playerTransform.prevPos = playerTransform.pos;

	// Player movement

	if (input.up)
	{
		playerVelocity.y -= 5;
	}
	else if (input.down)
	{
		playerVelocity.y += 5;
	}

	else if (input.right)
	{
		playerVelocity.x += 5;
	}
	else if (input.left)
	{
		playerVelocity.x -= 5;
	}
	
	playerTransform.vel = playerVelocity;

	for (auto& e : m_entityManager.getEntities())
	{
		auto& ct = e->getComponent<CTransform>();
		ct.pos += ct.vel;
	}
}

void ScenePlay::onEnd()
{
}

void ScenePlay::sCollision()
{
	auto& playerTransform = player->getComponent<CTransform>();
	for (auto& e : m_entityManager.getEntities("wall"))
	{

		auto overLap = phy.getOverlap(player, e);
		auto prevOverLap = phy.getPrevOverlap(player, e);
		auto ePos = e->getComponent<CTransform>().pos;
		/*std::cout << overLap.x << " " << overLap.y << std::endl;
		std::cout << prevOverLap.x << " p " << prevOverLap.y << std::endl;*/

		if (overLap.x > 0 && overLap.y > 0)
		{
			if (prevOverLap.y > 0 && playerTransform.pos.x > ePos.x)
			{
				playerTransform.pos += Vec2(overLap.x, 0);
			}
			else if (prevOverLap.y > 0 && playerTransform.pos.x < ePos.x)
			{
				playerTransform.pos += Vec2(-overLap.x, 0);
			}

			if (prevOverLap.x > 0 && playerTransform.pos.y < ePos.y)
			{
				playerTransform.pos += Vec2(0, -overLap.y);
			}
			else if (prevOverLap.x > 0 && playerTransform.pos.y > ePos.y)
			{
				playerTransform.pos += Vec2(0, overLap.y);
			}
			playerTransform.prevPos = playerTransform.pos;
			
			break;
		}

	}
	   
}

void ScenePlay::sRayCasting()
{
	setRayForPlayer(player);
	auto& pPos = player->getComponent<CTransform>();
	auto& pVx = player->getComponent<CVertex>();
	for (auto& e : m_entityManager.getEntities("torch"))
	{
		auto& ePos = e->getComponent<CTransform>();
		auto& eL = e->getComponent<CLight>();
		if (pPos.pos.dist(ePos.pos) <= eL.length)
		{
			pVx.update(pPos.pos);
			setRayForTorch(e);
		}
	}
}

void ScenePlay::updateRay(std::shared_ptr<Entity> e)
{

	//TODO suck
	auto& window = m_game->window();
	if (!e->hasComponent<CLight>()) return;
	auto& pPos = e->getComponent<CTransform>().pos;
	auto& pl = e->getComponent<CLight>();
	pl.ray.clear();

	if (pl.angle.empty()) return;
	phy.IntersectRay( pl.angle, sf::Vector2f(pPos.x, pPos.y), m_entityManager.getEntities("wall"));

	//sort all ray vector with scope
	phy.sortVector(pl.angle, pPos, m_pos, pl.scope);

	//add the ligth
	phy.addLight(pl.light, pl.angle, pPos, 0);

	if (m_drawRay) phy.addRay(pl.ray, pl.angle, pPos);
}

void ScenePlay::setRayForPlayer(std::shared_ptr<Entity> e)
{
	auto& window = m_game->window();
	if (!e->hasComponent<CLight>()) return;
	auto& pPos = e->getComponent<CTransform>().pos;
	auto& pl = e->getComponent<CLight>();
	pl.ray.clear();
	pl.angle.clear();
	phy.getRectanglePoints(pl.angle, m_entityManager.getEntities("wall"), pPos, m_pos, pl.scope, pl.length);
	phy.getAllDirection(pl.angle, 36, pPos, m_pos, pl.scope, pl.length);

	if (pl.angle.empty()) return;
	phy.IntersectRay(pl.angle, sf::Vector2f(pPos.x, pPos.y), m_entityManager.getEntities("wall"));

	//sort all ray vector with scope
	phy.sortVector(pl.angle, pPos, m_pos, pl.scope);

	//adding light effect
	phy.lightEffect(pl.angle, pPos, pl.length);

	//add the ligth
	//phy.addLight(pl.light, pl.angle, pPos, 1);
	phy.addLight(pl.light, pl.angle, pPos, 0);

	if (m_drawRay) phy.addRay(pl.ray, pl.angle, pPos);
}

void ScenePlay::setRayForTorch(std::shared_ptr<Entity> e)
{
	auto& window = m_game->window();
	if (!e->hasComponent<CLight>()) return;
	auto& pPos = e->getComponent<CTransform>().pos;
	auto& pl = e->getComponent<CLight>();
	pl.ray.clear();
	pl.angle.clear();
	phy.getRectanglePoints(pl.angle, m_entityManager.getEntities("vertex_object"), pPos, m_pos, pl.scope, pl.length);

	if (pl.angle.empty()) return;
	phy.IntersectRay(pl.angle, sf::Vector2f(pPos.x, pPos.y), m_entityManager.getEntities("vertex_object"));

	//sort all ray vector with scope
	phy.sortVector(pl.angle, pPos, m_pos, pl.scope);

	//adding light effect
	phy.lightEffect(pl.angle, pPos, pl.length);

	//add the ligth
	phy.addLight(pl.light, pl.angle, pPos, 1);

	if (m_drawRay) phy.addRay(pl.ray, pl.angle, pPos);
}

void ScenePlay::readMap()
{
	std::ifstream in("res/world/level_1.json");

	Json::Value val;
	Json::Reader reader;

	reader.parse(in, val);

	int w = val["width"].asInt();
	int h = val["height"].asInt();
	int s = val["size"].asInt();

	auto& TILE = val["data"];
	
	int col = 0, row = 0, i = 0;
	while (row < h)
	{
		if (TILE[i].asInt() == 1)
		{
			auto wall = m_entityManager.addEntity("wall");
			
			Vec2 pos((col * s) + s / 2, (row * s) + s/2);

			wall->addComponent<CTransform>(pos, Vec2(0, 0)); 
			auto& ct = wall->getComponent<CTransform>();

			wall->addComponent<CBoundingBox>(Vec2(s, s));
			auto& cb = wall->getComponent<CBoundingBox>();

			wall->addComponent<CVertex>(pos, Vec2(s, s));

			m_entityManager.addEntity(wall, "vertex_object");
		}
		else if(TILE[i].asInt() == 2)
		{
			spawnTorch(col, row, s, true);
		}
		col++;
		i++;
		if (col >= w)
		{
			col = 0;
			row++;
		}
	}
	m_entityManager.update();

	for (auto& e : m_entityManager.getEntities("torch"))
	{
		setRayForTorch(e);
	}

	in.close();
}

void ScenePlay::testMap()
{

	auto torch2 = m_entityManager.addEntity("torch");
	torch2->addComponent<CTransform>(Vec2(400, 300), Vec2(0, 0));
	torch2->addComponent<CBoundingBox>(Vec2(8, 8));
	torch2->addComponent<CLight>(360, 250);

	auto wall = m_entityManager.addEntity("wall");
	wall->addComponent<CTransform>(Vec2(300, 150), Vec2(0, 0));
	auto& ct = wall->getComponent<CTransform>();
	wall->addComponent<CBoundingBox>(Vec2(16, 16));
	auto& cb = wall->getComponent<CBoundingBox>();
	wall->addComponent<CVertex>(Vec2(300, 150), Vec2(16, 16));

	m_entityManager.addEntity(wall, "vertex_object");
}

void ScenePlay::spawnTorch(const int col, const int row, const int s, const bool init)
{
	Vec2 pos((col * s) + s / 2, (row * s) + s / 2);
	//std::cout << col << " " << row << " size: " << s << std::endl;
	auto torch = m_entityManager.addEntity("torch");
	torch->addComponent<CTransform>(pos, Vec2(0, 0));
	torch->addComponent<CLight>(360, 250);

	if(!init) setRayForTorch(torch);
}






