#pragma once
#include <vector>
#include <map>
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
	Vec2 prevPos = Vec2(0,0);

	Vec2 vel = Vec2(1,1);
	CTransform()
	{

	}
	CTransform(const Vec2& _pos, const Vec2& _vel)
		:pos(_pos),
		vel(_vel),
		prevPos(_pos)
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
	std::vector<sf::Vertex> vertex;
	Vec2 m_size;
	CVertex(){}
	CVertex(const Vec2& pos, const Vec2& size) 
		:m_size(size)
	{

		float ax = pos.x - m_size.x/2;
		float ay = pos.y - m_size.y/2;
		vertex.push_back(sf::Vertex(sf::Vector2f(ax, ay), sf::Color::Green));
		vertex.push_back(sf::Vertex(sf::Vector2f(ax, ay + m_size.y), sf::Color::Green));
		vertex.push_back(sf::Vertex(sf::Vector2f(ax + m_size.x, ay + m_size.y), sf::Color::Green));
		vertex.push_back(sf::Vertex(sf::Vector2f(ax + m_size.x, ay), sf::Color::Green));
	}

	void update(const Vec2& pos)
	{
		vertex.clear();
		float ax = pos.x - m_size.x / 2;
		float ay = pos.y - m_size.y / 2;
		vertex.push_back(sf::Vertex(sf::Vector2f(ax, ay), sf::Color::Green));
		vertex.push_back(sf::Vertex(sf::Vector2f(ax, ay + m_size.y), sf::Color::Green));
		vertex.push_back(sf::Vertex(sf::Vector2f(ax + m_size.x, ay + m_size.y), sf::Color::Green));
		vertex.push_back(sf::Vertex(sf::Vector2f(ax + m_size.x, ay), sf::Color::Green));
	}
};


class CLight : public Component
{
public:
	float scope = 120.0f;
	float length = 250;

	std::vector<sf::Vertex*> ray;
	std::vector<sf::Vertex*> line;
	std::vector<sf::Vertex> surfaceLine;
	std::vector<sf::Vertex> angle;
	std::vector<CVertex> staticVec;
	sf::VertexArray light;
	CLight(){}
	CLight(const float _scope, const float _length)
		:scope(_scope),
		length(_length)
	{

	}
};

class CItem : public Component
{
public:
	std::map<std::string, int> items;
	CItem(){}
	CItem(const std::string& tag)
	{
		items[tag] = 0;
	}
};

class CState : public Component
{
public:
	bool m_drawAble = false;
	bool m_canPassThrough = false;
	CState(){ }
};