#include "Action.h"

Action::Action()
{
}

Action::Action(const std::string& _type, const std::string& _name)
	:name(_name)
	, type(_type)
{
}

const std::string& Action::getName()
{
	return name;
}

const std::string& Action::getType()
{
	return type;
}
