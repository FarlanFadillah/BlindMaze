#pragma once
#include <vector>
#include "Vec2.hpp"
class Component
{
public:
	bool has = true;
};

class CTransform : public Component
{
public:
	Vec2 pos;
	CTransform()
	{

	}
	CTransform(const Vec2& _pos)
		:pos(_pos)
	{
	}
};

class CNode : public Component
{
public:
	std::vector<Vec2> body;
	size_t size = 16;
	CNode(){}
	CNode(const Vec2& init)
	{
		body.push_back(init);
		body.push_back(Vec2(init.x + 1, init.y));
	}
};


class CState : public Component
{
public:
	std::string state = "left";
	CState()
	{
	}

};