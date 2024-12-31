#pragma once
#include <string>
class Action
{
protected:
	std::string name = "NONE";
	std::string type = "NONE";
public:

	Action();
	Action(const std::string & _type, const std::string & _name);

	const std::string& getName();
	const std::string& getType();
};

