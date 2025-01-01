#include "ScenePlay.h"


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


	player = m_entityManager.addEntity("player");

	player->addComponent<CTransform>(Vec2(100, 300), Vec2(5, 5));
	player->addComponent<CBoundingBox>(Vec2(16, 16));
	player->addComponent<CVertex>(Vec2(100, 100), Vec2(16, 16));
	player->addComponent<CInput>();
	player->addComponent<CLight>(120, 250);


	auto torch = m_entityManager.addEntity("torch");
	torch->addComponent<CTransform>(Vec2(400, 100), Vec2(0, 0));
	torch->addComponent<CBoundingBox>(Vec2(8, 8));
	torch->addComponent<CLight>(360, 250);
	
	auto torch2 = m_entityManager.addEntity("torch");
	torch2->addComponent<CTransform>(Vec2(400, 300), Vec2(0, 0));
	torch2->addComponent<CBoundingBox>(Vec2(8, 8));
	torch2->addComponent<CLight>(360, 250);

	//set up wall

	auto wall = m_entityManager.addEntity("wall");

	wall->addComponent<CTransform>(Vec2(150, 150), Vec2(0, 0)); 
	auto& ct = wall->getComponent<CTransform>(); 

	wall->addComponent<CBoundingBox>(Vec2(128, 128));
	auto& cb = wall->getComponent<CBoundingBox>();

	wall->addComponent<CVertex>(Vec2(150, 150), Vec2(128, 128));

	


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
		sRayCasting();
	}

	sRender();
}

void ScenePlay::sRender()
{
	auto& window = m_game->window();
	window.clear(sf::Color::Black);

	//draw all entities

	for (auto& e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLight>())
		{
			auto& pl = e->getComponent<CLight>();
			auto& ray = pl.ray;
			auto light = pl.light;
			if (m_drawLight)
			{
				for (auto& l : light)
				{
					window.draw(l, 3, sf::Triangles);
				}
			}
			if (m_drawRay)
			{
				//window.draw(visibilityPolygon);
				for (auto& e : ray)
				{
					window.draw(e, 2, sf::Lines);
				}
			}
		}
	}
	

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
			rect.setFillColor((e == player) ? sf::Color::White : sf::Color::Transparent);

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
	}
}

void ScenePlay::sMovement()
{
	Vec2 playerVelocity(0, 0);

	auto& input = player->getComponent<CInput>();
	auto& playerTransform = player->getComponent<CTransform>();


	// Player movement

	if (input.up)
	{
		playerVelocity.y -= 5;
	}
	else if (input.down)
	{
		playerVelocity.y += 5;
	}

	if (input.right)
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

void ScenePlay::sRayCasting()
{
	auto& window = m_game->window();
	int i = 0;
	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CLight>()) continue;
		auto& pPos = e->getComponent<CTransform>().pos;
		auto& pl = e->getComponent<CLight>();
		pl.ray.clear();
		pl.angle.clear();
		phy.getRectanglePoints(pl.angle, m_entityManager.getEntities("wall"), e, window, m_pos, pl.scope, pl.length);
		phy.getAllDirection(pl.angle, 360, e, m_pos, pl.scope, pl.length);

		phy.IntersectRay(pl.angle, pPos, m_entityManager.getEntities("wall"));
		
		//std::cout << phy.vectorToDegree(pPos, m_pos) << std::endl; 
		 
		
		//sort all ray vector with scope
		phy.sortVector(pl.angle, pPos, m_pos, pl.scope);

		//adding light effect
		phy.lightEffect(pl.angle, e, pl.length);
		//std::cout << angle.size() << std::endl;

		//add the ligth
		if (e->tag() == "player")
		{
			phy.addLight(pl.light, pl.angle, pPos, 1);
		}
		else
		{
			phy.addLight(pl.light, pl.angle, pPos, 0);
		}

		// add the ray
		phy.addRay(pl.ray, pl.angle, pPos);

		//std::cout << ray.size() << std::endl;
	}
	

}


