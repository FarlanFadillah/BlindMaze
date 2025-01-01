#pragma once
#include <vector>
#include "Vec2.hpp"
#include <SFML/Graphics.hpp>
class Component
{
public:
	bool has = false;
};

class CTransform : public Component
{
public:
	Vec2 pos = Vec2(0,0);
	Vec2 vel = Vec2(1,1);
	CTransform()
	{

	}
	CTransform(const Vec2& _pos, const Vec2& _vel)
		:pos(_pos),
		vel(_vel)
	{
	}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;

	CBoundingBox()
	{

	}
	CBoundingBox(const Vec2& _size)
		:size(_size)
	{
		halfSize = Vec2(size.x / 2, size.y / 2);
	}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	CInput()
	{
	}
};

class CVertex : public Component
{
public:
	sf::Vertex* vertex;

	CVertex(){}
	CVertex(const Vec2& pos, const Vec2& size) {

		float ax = pos.x - size.x/2;
		float ay = pos.y - size.y/2;

		vertex = new sf::Vertex[4]{
				sf::Vertex(sf::Vector2f(ax, ay), sf::Color::Green),
				sf::Vertex(sf::Vector2f(ax, ay + size.y), sf::Color::Green),
				sf::Vertex(sf::Vector2f(ax + size.x, ay + size.y), sf::Color::Green),
				sf::Vertex(sf::Vector2f(ax + size.x, ay), sf::Color::Green)
		};
	}
};


class CLight : public Component
{
public:
	float scope = 120.0f;
	float length = 250;

	std::vector<sf::Vertex*> ray;
	std::vector<sf::Vertex*> light;
	std::vector<sf::Vertex> angle;
	CLight(){}
	CLight(const float _scope, const float _length)
		:scope(_scope),
		length(_length)
	{

	}
};