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

#include "new.hpp"
#include "entity.hpp"
#include "replication.hpp"
#include "controller.hpp"
#include "factory.hpp"
#include "configmanager.hpp"
#include "console.hpp"
#include "physicengine.hpp"
#include "networkengine.hpp"
#include "graphicengine.hpp"
#include "eventmanager.hpp"
#include "callbackmanager.hpp"
#include "interpreter.hpp"
#include "gameengine.hpp"

#include <iostream>

Gameengine::Gameengine(Gameengine::Option const &opt) : master(opt.master), ctrlid(-1), _entsize(1024), _entities(0), callback(0), physic(0), network(0), console(0), event(0), graphic(0), controllerclass()
{
	Factory::get_instance().generate_type();

	_entities = new Entity*[_entsize];
	for (unsigned int i = 0; i < _entsize; ++i)
		_entities[i] = 0;
	
	callback = new Callbackmanager();
	physic = new Physicengine();
	interpreter = new Interpreter(this);
	interpreter->exec("help");

	if (!opt.local)
	{
		network = new Networkengine(this);
		network->connect(opt.ip, opt.port);
	}

	Df_node	*nd = Configmanager::get_instance().get("game.df");

	if (opt.graphic)
	{
		console = new Console(this);
		event = new Eventmanager(this);
		graphic = new Graphicengine();
	}

	controllerclass = nd->safe_get("controller", Df_node::STRING, 1)->cstr[0];

	if (master)
		create("World", 0, true);
	if (!network)
	{
		Controller *ctrl = (Controller *)create(controllerclass, 0, true);
		ctrlid = ctrl->id;
	}
}

Gameengine::~Gameengine()
{
	for (unsigned int i = 0; i < _entsize; ++i)
		if (_entities[i])
			delete _entities[i];
	delete [] _entities;

	delete callback;
	delete physic;

	if (network)
		delete network;
	
	
	if (graphic)
	{
		delete event;
		delete graphic;
	}
	delete console;
}

Entity		*Gameengine::create(std::string const &name, Entity const *owner, bool const need_replication)
{
	int		id = -1;

	for (unsigned int i = 0; i < _entsize; ++i)
	{
		if (!_entities[i])
		{
			id = i;
			break;
		}
	}
	if (id < 0)
	{
		id = _entsize;
		_entities = resize(_entities, _entsize, _entsize << 1);
		_entsize <<= 1;
		for (unsigned int i = _entsize >> 1; i < _entsize; ++i)
			_entities[i] = 0;
	}
	_entities[id] = Factory::get_instance().create(this, (need_replication && network ? network->new_replication(id) : 0), id, name, owner);
	return (_entities[id]);
}

Entity		*Gameengine::create(Replication *r)
{
	_entities[r->id] = Factory::get_instance().create(this, r);
	return (_entities[r->id]);
}

Entity	*Gameengine::find_entity(int const id)
{
	return (id >= 0 && id < (int)_entsize ? _entities[id] : 0);
}

void	Gameengine::notify_owner(Entity *a, bool const l)
{
	if (a->ownerid < (int)_entsize && _entities[a->ownerid])
		_entities[a->ownerid]->notified_by_owned(a, l);
}

void	Gameengine::notify_owned(Entity *a, bool const l)
{
	for	(unsigned int i = 0; i < _entsize; ++i)
	{
		if (_entities[i]->ownerid == a->id)
			_entities[i]->notified_by_owner(a, l);
	}
}

void			Gameengine::control(int const id)
{
	Controller	*ctrl;

	ctrlid = id;
	if ((ctrl = (Controller *)find_entity(id)) && ctrl->state == Entity::OK)
		ctrl->bind();
}

void	Gameengine::tick(float const delta)
{
	//std::cout << "em" << std::endl;
	if (event)
		event->event();
	//std::cout << "cl" << std::endl;
	if (console)
		console->tick(delta);
	//std::cout << "ne" << std::endl;
	if (network)
		network->tick(delta);
	//std::cout << "pe" << std::endl;
	physic->tick(delta);
	//std::cout << "cb" << std::endl;
	callback->tick(delta);
	//std::cout << "ac" << std::endl;
	for (unsigned int i = 0; i < _entsize; ++i)
	{
		if (_entities[i])
		{
			switch (_entities[i]->state)
			{
				case Entity::CREATED:
					_entities[i]->postinstanciation();
				case Entity::OK:
					_entities[i]->tick(delta);
					break;
				case Entity::DESTROYED:
					delete _entities[i];
					_entities[i] = 0;
					break;
			};
		}
	}
	//std::cout << "ge" << std::endl;
	if (console)
		console->draw();
	if (graphic)
		graphic->tick(delta);
	//std::cout << "end" << std::endl;
}
