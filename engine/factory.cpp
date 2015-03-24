#include <iostream>
#include <stdlib.h>
#include "factory.hpp"

Factory				&Factory::get_instance()
{
	static Factory	instance;

	return (instance);
}

void		Factory::register_class(std::string const &name, CF cf)
{
	Class	a;

	a.type = -1;
	a.cf = cf;
	_classmap[name] = a;
}

void										Factory::generate_type()
{
	std::map<std::string, Class>::iterator	i;
	short int								type = 0;

	for (i = _classmap.begin(); i != _classmap.end(); ++i)
		i->second.type = ++type;
}

short int									Factory::get_type(std::string const &name) const
{
	std::map<std::string, Class>::const_iterator	i;

	if ((i = _classmap.find(name)) != _classmap.end())
		return (i->second.type);
	else
    {
		std::cerr << "Error: Factory::get_type() fails to find class: " << name << std::endl;
		exit(EXIT_FAILURE);
		return (-1);
	}
}

Actor										*Factory::create(Actormanager *am, short int type, Replication *r) const
{
	std::map<std::string, Class>::const_iterator	i;

	for (i = _classmap.begin(); i != _classmap.end(); ++i)
	{
		if (i->second.type == type)
			return (i->second.cf(am, r, i->second.type, 0, 0));
	}
	std::cerr << "Error: Factory::create() fails to find id: " << type << std::endl;
	exit(EXIT_FAILURE);
	return (0);
}

Actor												*Factory::create(Actormanager *am, std::string const &name, int id, Actor const *owner) const
{
	std::map<std::string, Class>::const_iterator	i;

	if ((i = _classmap.find(name)) != _classmap.end())
		return (i->second.cf(am, 0, i->second.type, id, owner));
	else
    {
		std::cerr << "Error: Factory::create() fails to find class: " << name << std::endl;
		exit(EXIT_FAILURE);
		return (0);
	}
}
