#include "Entity.hpp"

Entity::Entity(const size_t id, const std::string& tag)
	: m_id(id)
	,m_tag(tag)
{
}

const std::string& Entity::tag() const
{
	return m_tag;
}

bool Entity::isActive() const
{
	return m_active;
}

const size_t Entity::id() const
{
	return m_id;
}

void Entity::destroy()
{
	m_active = false;
}

const std::string& Entity::name() const
{
	return m_name; 
}

void Entity::setName(std::string name)
{
	m_name = name;
}
