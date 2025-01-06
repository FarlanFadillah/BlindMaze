#include "ScenePlay.h"
#include <fstream>
#include <json/json.h>


ScenePlay::ScenePlay()
{
}

ScenePlay::ScenePlay(Engine* engine, const int _level, const std::string& config)
	:Scene(engine),
	level(_level)
{
	init(config);
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");

	if (debugMode)
	{
		registerAction(sf::Keyboard::F1, "DRAW_LIGHT");
		registerAction(sf::Keyboard::F2, "DRAW_RAY");
		registerAction(sf::Keyboard::F3, "DRAW_ENTITIES");
		registerAction(sf::Keyboard::F4, "DRAW_MOUSEPOS");
		registerAction(sf::Keyboard::F5, "TRANSPARENT_BOX");
		registerAction(sf::Keyboard::F6, "COLLISION");
		registerAction(sf::Keyboard::F7, "NEXT_LEVEL");
	}

	registerAction(sf::Keyboard::F, "PLAYER_LIGHT");

	ostrich_regular.loadFromFile("res/fonts/ostrich-regular.ttf");

	initLevel("res/config/levels.ini");
	imageToMap(levels.at(level));
	
	auto& window = m_game->window();
	CameraView.setSize(window.getSize().x, window.getSize().y);
	CameraView.setCenter(window.getSize().x / 2, window.getSize().y / 2);
	window.setView(CameraView);

}

void ScenePlay::init(const std::string& config)
{
	std::ifstream in(config);
	Json::Value val;
	Json::Reader read;
	read.parse(in, val);
	m_drawEntities = val["m_drawEntities"].asBool();
	m_drawPlayer = val["m_drawPlayer"].asBool();
	m_drawMousePos = val["m_drawMousePos"].asBool();
	m_drawLight = val["m_drawLight"].asBool();
	m_playerLight = val["m_playerLight"].asBool();
	m_drawRay = val["m_drawRay"].asBool();
	m_drawGui = val["m_drawGui"].asBool();
	m_transParentBox = val["m_transParentBox"].asBool();
	m_collision = val["m_collision"].asBool();
	debugMode = val["debugMode"].asBool();
}



void ScenePlay::update()
{
	objective();
	if (!m_paused)
	{
		m_entityManager.update();
		sMovement();
		if(m_collision) sCollision();
		sRayCasting();
		sCamera();
	}

	sRender();
}

void ScenePlay::sRender()
{
	auto& window = m_game->window();
	window.clear(sf::Color::Black);



	//draw player light
	if (m_drawPlayer)
	{
		auto& pl = player->getComponent<CLight>();
		auto& pItem = player->getComponent<CItem>();
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
		for (auto& e : pl.line)
		{
			window.draw(e, 2, sf::Lines); 
		}

		auto& ct = player->getComponent<CTransform>();
		auto& bb = player->getComponent<CBoundingBox>();

		sf::CircleShape rect; 
		rect.setRadius(bb.size.x/2);
		rect.setPosition(ct.pos.x, ct.pos.y);
		rect.setOrigin(bb.size.x / 2, bb.size.y / 2);
		rect.setFillColor(sf::Color::White);

		
		window.draw(rect);
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
		for (auto& e : m_entityManager.getEntities("wall"))
		{
			if (!e->hasComponent<CBoundingBox>()) continue;
			auto& ct = e->getComponent<CTransform>();
			auto& bb = e->getComponent<CBoundingBox>();

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(bb.size.x, bb.size.y));
			rect.setPosition(ct.pos.x, ct.pos.y);
			rect.setOrigin(bb.size.x / 2, bb.size.y / 2);
			rect.setFillColor(m_transParentBox ? sf::Color(0, 255, 0, 0) : sf::Color(0, 255, 0, 255));

			window.draw(rect);
		}
	}

	// draw key items
	for (auto& e : m_entityManager.getEntities("key"))
	{
		if (!e->hasComponent<CState>()) continue;

		auto& eState = e->getComponent<CState>();
		auto& eTransform = e->getComponent<CTransform>();
		if (eState.m_drawAble)
		{
			sf::RectangleShape rect(sf::Vector2f(8,8));
			rect.setPosition(eTransform.pos.x, eTransform.pos.y);
			rect.setOrigin(sf::Vector2f(4, 4));
			rect.setFillColor(sf::Color::Yellow);

			window.draw(rect);
		}
		eState.m_drawAble = false;
	}

	for (auto& e : m_entityManager.getEntities("door"))
	{
		if (!e->hasComponent<CBoundingBox>()) continue;
		auto& ct = e->getComponent<CTransform>();
		auto& bb = e->getComponent<CBoundingBox>();
		auto& cs = e->getComponent<CState>();

		sf::RectangleShape rect; 
		rect.setSize(sf::Vector2f(bb.size.x, bb.size.y)); 
		rect.setPosition(ct.pos.x, ct.pos.y); 
		rect.setOrigin(bb.size.x / 2, bb.size.y / 2); 
		rect.setFillColor(cs.m_drawAble ? cs.m_canPassThrough ? sf::Color::Green : sf::Color::Blue : sf::Color::Transparent);

		window.draw(rect); 
		cs.m_drawAble = false;
	}

	if (m_drawGui)
	{
		auto& pItem = player->getComponent<CItem>();
		//draw key count
		sf::Text text;
		text.setFont(ostrich_regular);
		text.setFillColor(sf::Color::White);
		text.setPosition(windowToWorldPos(sf::Vector2f(32, 32)));
		text.setOrigin(16, 16);
		text.setString("Key : " + std::to_string(pItem.items.at("key")));
		window.draw(text);

		text.setString("Level " + std::to_string(level + 1));
		text.setPosition(windowToWorldPos(sf::Vector2f(window.getSize().x - 64, 32)));
		window.draw(text);
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
			m_wpos = windowToWorldPos(m_pos);
			mouseMoved = true; 
		}
		else if (action.name() == "PLAYER_LIGHT")
		{
			m_playerLight = !m_playerLight;
		}
		else if (action.name() == "MOUSE_CLICKED")
		{
			//std::cout << action.pos().x << " " << action.pos().y << std::endl;
			//auto& grid = phy.getRectGrid(Vec2(action.pos().x, action.pos().y), 32, false);
			//spawnTorch(grid.x, grid.y, 32, false);
			doorCollision(m_wpos, "door"); 
		}
		else if (action.name() == "NEXT_LEVEL")
		{
			m_numDoor = 0;
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
	
	wallCollision("wall");
	wallCollision("door");
	itemCollision("key");
}

void ScenePlay::sCamera()
{
	auto& window = m_game->window();
	
	auto& playerPos = player->getComponent<CTransform>().pos;

	float distX = playerPos.x - CameraView.getCenter().x;
	float distY = playerPos.y - CameraView.getCenter().y;
	float scale;

	CameraView.move(sf::Vector2f(distX, distY));
	window.setView(CameraView);
	if (!mouseMoved) m_wpos += sf::Vector2f(distX, distY); 
	mouseMoved = false;
}

void ScenePlay::objective()
{
	if (m_numDoor <= 0)
	{
		if (level < levels.size() - 1)
		{
			m_game->changeScene("MAIN", std::make_shared<ScenePlay>(m_game, level + 1, "res/config/default.json"));
		}
		else
		{
			m_game->changeScene("MAIN", std::make_shared<ScenePlay>(m_game, 0, "res/config/default.json"));
		}
	}
}

void ScenePlay::sRayCasting()
{
	setRayForPlayer(player);
	auto& pPos = player->getComponent<CTransform>();
	auto& pVx = player->getComponent<CVertex>();
	auto& pL = player->getComponent<CLight>();

	//update ray for torch ( when ligth intersect with a player)
	for (auto& e : m_entityManager.getEntities("torch"))
	{
		auto& ePos = e->getComponent<CTransform>();
		auto& eL = e->getComponent<CLight>();
		if (pPos.pos.dist(ePos.pos) <= eL.length)
		{
			pVx.update(pPos.pos);
			updateTorchRay(e);
		}
	}

	// spot the keys
	auto& light = pL.light;
	for (auto& e : m_entityManager.getEntities("dark_obj"))
	{
		if (m_playerLight && phy.spotLight(pL.angle, sf::Vector2f(pPos.pos.x, pPos.pos.y), e)) e->getComponent<CState>().m_drawAble = true;
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
	phy.sortVector(pl.angle, pPos, m_wpos, pl.scope);

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
	pl.angle.clear();
	phy.getRectanglePoints(pl.angle, m_entityManager.getEntities("non_transparent"), pPos, m_wpos, pl.scope, pl.length);
	phy.getRectanglePoints(pl.angle, m_entityManager.getEntities("transparent"), pPos, m_wpos, pl.scope, pl.length);
	phy.getAllDirection(pl.angle, 36, pPos, m_wpos, pl.scope, pl.length);

	if (pl.angle.empty()) return;
	phy.IntersectRay(pl.angle, sf::Vector2f(pPos.x, pPos.y), m_entityManager.getEntities("non_transparent"));

	//sort all ray vector with scope
	phy.sortVector(pl.angle, pPos, m_wpos, pl.scope);

	//adding light effect
	phy.lightEffect(pl.angle, pPos, pl.length);

	//add the ligth
	//phy.addLight(pl.light, pl.angle, pPos, 1);
	phy.addLight(pl.light, pl.angle, pPos, 0);

	if (m_drawRay) phy.addRay(pl.ray, pl.angle, pPos);

	//phy.addSurfaceLine(pl.line, pl.surfaceLine);
}

void ScenePlay::updateTorchRay(std::shared_ptr<Entity> e)
{
	if (!e->hasComponent<CLight>()) return;
	auto& pPos = e->getComponent<CTransform>().pos;
	auto& pl = e->getComponent<CLight>();
	pl.angle.clear();
	phy.setStaticRectanglePoints(pl.angle, pl.staticVec, pPos, m_wpos, pl.scope, pl.length);
	phy.getRectanglePoints(pl.angle, m_entityManager.getEntities("player"), pPos, m_wpos, pl.scope, pl.length);

	if (pl.angle.empty()) return;
	phy.IntersectStaticRay(pl.angle, sf::Vector2f(pPos.x, pPos.y), pl.staticVec);
	phy.IntersectRay(pl.angle, sf::Vector2f(pPos.x, pPos.y), m_entityManager.getEntities("player"));

	//sort all ray vector with scope
	phy.sortVector(pl.angle, pPos, m_wpos, pl.scope);

	//adding light effect
	phy.lightEffect(pl.angle, pPos, pl.length);

	//add the ligth
	phy.addLight(pl.light, pl.angle, pPos, 1);

	if (m_drawRay) phy.addRay(pl.ray, pl.angle, pPos);

}

void ScenePlay::setRayForTorch(std::shared_ptr<Entity> e)
{
	auto& window = m_game->window();
	if (!e->hasComponent<CLight>()) return;
	auto& pPos = e->getComponent<CTransform>().pos;
	auto& pl = e->getComponent<CLight>();
	pl.angle.clear();
	phy.getStaticRectanglePoints(pl.staticVec, m_entityManager.getEntities("non_transparent"), pPos, m_wpos, pl.scope, pl.length);
	phy.setStaticRectanglePoints(pl.angle, pl.staticVec, pPos, m_wpos, pl.scope, pl.length);

	if (pl.angle.empty()) return;
	phy.IntersectStaticRay(pl.angle, sf::Vector2f(pPos.x, pPos.y), pl.staticVec);
	phy.IntersectRay(pl.angle, sf::Vector2f(pPos.x, pPos.y), m_entityManager.getEntities("player"));

	//sort all ray vector with scope
	phy.sortVector(pl.angle, pPos, m_wpos, pl.scope);

	//adding light effect
	phy.lightEffect(pl.angle, pPos, pl.length);

	//add the ligth
	phy.addLight(pl.light, pl.angle, pPos, 1);

	if (m_drawRay) phy.addRay(pl.ray, pl.angle, pPos);
}


void ScenePlay::readMap(const std::string& level)
{
	std::ifstream in(level);

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
		if (TILE[i].asInt() == 1 || TILE[i].asInt() == 4)
		{
			spawnWall(col, row, s, TILE[i].asInt() == 4);
		}
		else if(TILE[i].asInt() == 2)
		{
			spawnTorch(col, row, s, true);
		}
		else if (TILE[i].asInt() == 3)
		{
			spawnKey(col, row, s);
		}
		else if (TILE[i].asInt() == 5)
		{
			spawnDoor(col, row, s);
			m_numDoor++;
		}
		else if (TILE[i].asInt() == 9)
		{
			spawnPlayer(col, row, s);
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

void ScenePlay::imageToMap(const std::string& fileName)
{

	sf::Image img;
	if (!img.loadFromFile(fileName))
	{
		std::cout << "Failed to read the map0\n";
		return;
	}
	
	int W = img.getSize().x, H = img.getSize().y;
	int col = 0, row = 0;
	int s = 32;
	while (row < H)
	{
		sf::Color color = img.getPixel(col, row);
		int r = color.r, g = color.g, b = color.b;
		if (r == 0 && g == 255 && b == 0 || r == 0 && g == 255 && b == 255)
		{
			spawnWall(col, row, s, r == 0 && g == 255 && b == 255);
		}
		else if (r == 255 && g == 255 && b == 255)
		{
			spawnTorch(col, row, s, true);
		}
		else if (r == 255 && g == 255 && b == 0)
		{
			spawnKey(col, row, s);
		}
		else if (r == 0 && g == 0 && b == 255)
		{
			spawnDoor(col, row, s);
			m_numDoor++;
		}
		else if (r == 255 && g == 0 && b == 0)
		{
			spawnPlayer(col, row, s);
		}

		col++;
		if (col >= W)
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
}

void ScenePlay::wallCollision(const std::string& tag)
{
	auto& playerTransform = player->getComponent<CTransform>();
	for (auto& e : m_entityManager.getEntities(tag))
	{
		if (e->hasComponent<CState>() && e->getComponent<CState>().m_canPassThrough) continue;
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

void ScenePlay::itemCollision(const std::string& tag)
{
	auto& playerTransform = player->getComponent<CTransform>();
	auto& playerItems = player->getComponent<CItem>();
	for (auto& e : m_entityManager.getEntities(tag))
	{
		auto overLap = phy.getOverlap(player, e);
		auto prevOverLap = phy.getPrevOverlap(player, e);

		if (overLap.x > 0 && overLap.y > 0)
		{
			e->destroy();
			playerItems.items[tag]++;
		}
	}
}

void ScenePlay::doorCollision(const sf::Vector2f& pos, const std::string& tag)
{
	auto& playerTransform = player->getComponent<CTransform>();
	auto& playerItems = player->getComponent<CItem>();
	auto& playerL = player->getComponent<CLight>();
	for (auto& e : m_entityManager.getEntities(tag))
	{
		if (!e->hasComponent<CState>()) continue;
		auto ePos = e->getComponent<CTransform>().pos;
		auto& eState = e->getComponent<CState>();
		/*std::cout << overLap.x << " " << overLap.y << std::endl;
		std::cout << prevOverLap.x << " p " << prevOverLap.y << std::endl;*/

		if (phy.isInside(pos, e) && 
			!eState.m_canPassThrough && 
			!phy.outOfRange(32, sf::Vector2f(ePos.x, ePos.y), playerTransform.pos) &&
			playerItems.items["key"] > 0)
		{
			eState.m_canPassThrough = true;
			playerItems.items["key"]--;
			m_numDoor--;
			return;
		}
	}
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

	/*m_entityManager.addEntity(wall, "vertex_object_torch");
	m_entityManager.addEntity(wall, "vertex_object_pl");*/
}

void ScenePlay::spawnPlayer(const int col, const int row, const int s)
{
	player = m_entityManager.addEntity("player");
	Vec2 pos((col * s) + s / 2, (row * s) + s / 2);

	player->addComponent<CTransform>(pos, Vec2(5, 5));
	player->addComponent<CBoundingBox>(Vec2(16, 16));
	player->addComponent<CVertex>(pos, Vec2(16, 16));
	player->addComponent<CInput>();
	player->addComponent<CLight>(120, 250);
	player->addComponent<CItem>("key");
}

void ScenePlay::spawnWall(const int col, const int row, const int s, bool transparent)
{
	auto wall = m_entityManager.addEntity("wall");
	Vec2 pos((col * s) + s / 2, (row * s) + s / 2);

	wall->addComponent<CTransform>(pos, Vec2(0, 0));
	wall->addComponent<CBoundingBox>(Vec2(s, s));

	wall->addComponent<CVertex>(pos, Vec2(s, s));
	transparent ? m_entityManager.addEntity(wall, "transparent") : m_entityManager.addEntity(wall, "non_transparent");
}

void ScenePlay::spawnDoor(const int col, const int row, const int s)
{
	auto door = m_entityManager.addEntity("door");
	Vec2 pos((col * s) + s / 2, (row * s) + s / 2);

	door->addComponent<CTransform>(pos, Vec2(0, 0));
	door->addComponent<CBoundingBox>(Vec2(s, s));

	door->addComponent<CVertex>(pos, Vec2(s, s));
	door->addComponent<CState>();
	m_entityManager.addEntity(door, "non_transparent");
	m_entityManager.addEntity(door, "dark_obj");
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

void ScenePlay::spawnKey(const int col, const int row, const int s)
{
	Vec2 pos((col * s) + s / 2, (row * s) + s / 2);
	auto key = m_entityManager.addEntity("key");
	key->addComponent<CTransform>(pos, Vec2(0, 0));
	key->addComponent<CBoundingBox>(Vec2(16, 16));
	key->addComponent<CVertex>(pos, Vec2(16, 16));
	key->addComponent<CState>();
	m_entityManager.addEntity(key, "transparent");
	m_entityManager.addEntity(key, "dark_obj");
}

void ScenePlay::initLevel(const std::string& config)
{
	std::fstream fin(config);
	while (fin.good())
	{
		std::string text;

		fin >> text;
		levels.push_back(text);
	}
}

sf::Vector2f ScenePlay::windowToWorldPos(const sf::Vector2f& pos)
{
	int cameraLeft = CameraView.getCenter().x - CameraView.getSize().x / 2;
	int cameraTop = CameraView.getCenter().y - CameraView.getSize().y / 2;

	return sf::Vector2f((pos.x + cameraLeft), pos.y + cameraTop);
}






