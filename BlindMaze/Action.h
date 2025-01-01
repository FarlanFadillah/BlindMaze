#pragma once
#include <string>
#include <SFML/Graphics.hpp>
class Action
{
protected:
	std::string m_name = "NONE";
	std::string m_type = "NONE";
	sf::Vector2f m_pos;
public:

	Action();
	Action(const std::string & _type, const std::string & _name);
	Action(const sf::Vector2f& pos);

	const std::string& name() const;
	const std::string& type() const;
	const sf::Vector2f& pos() const;
};

