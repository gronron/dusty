/******************************************************************************
Copyright (c) 2015, Geoffrey TOURON
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dusty nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

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
