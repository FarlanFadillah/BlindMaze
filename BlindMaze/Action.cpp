#include "Action.h"

Action::Action()
{
}

Action::Action(const std::string& _type, const std::string& _name)
	:m_name(_name)
	, m_type(_type)
{
}

Action::Action(const sf::Vector2f& pos)
	:m_pos(pos)
{
	m_type = "START";
	m_name = "MOUSE_MOVED";
}

const std::string& Action::name() const
{
	return m_name;
}

const std::string& Action::type() const
{
	return m_type;
}

const sf::Vector2f& Action::pos() const
{
	return m_pos;
}
