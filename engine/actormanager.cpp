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
#include "actor.hpp"
#include "replication.hpp"
#include "controller.hpp"
#include "factory.hpp"
#include "configmanager.hpp"
#include "console.hpp"
#include "physicengine.hpp"
#include "networkengine.hpp"
#include "graphicengine.hpp"
#include "eventmanager.hpp"
//#include "uimanager.hpp"
#include "actormanager.hpp"

#include <iostream>

Actormanager::Actormanager(Actormanager::Option const &opt) : cl(0), em(0), pe(0), ne(0), ge(0), master(opt.master), local(opt.local), graphic(opt.graphic), audio(opt.audio), _asize(0), _actors(0), controllerclass()
{
	Df_node	*nd;
	
	_asize = 64;
	_actors = new Actor*[_asize];
	for (unsigned int i = 0; i < _asize; ++i)
		_actors[i] = 0;

	nd = Configmanager::get_instance().get("game.df");
	nd->print();
	Factory::get_instance().generate_type();
	cl = new Console(this);
	em = new Eventmanager(this);
	pe = new Physicengine;
	if (!local)
	{
		ne = new Networkengine(this);
		ne->connect(opt.ip, opt.port);
	}
	if (graphic)
	{
		ge = new Graphicengine();
		//um = new Uimanager();
	}
	controllerclass = nd->safe_get("controller", Df_node::STRING, 1)->cstr[0];

	/*if (master)
		create(opt.level, 0, true);*/
	if (local)
	{
		Controller *ctrl = (Controller *)create(controllerclass, 0, true);
		_controllermap[ctrl->id] = ctrl;
		ctrl->bind();
	}
}

Actormanager::~Actormanager()
{
	for (unsigned int i = 0; i < _asize; ++i)
		if (_actors[i])
			delete _actors[i];
	if (graphic)
	{
		//delete um;
		delete ge;
	}
	if (ne)
		delete ne;
	delete pe;
	delete em;
	delete cl;
}

Actor		*Actormanager::create(std::string const &name, Actor const *owner, bool need_replication)
{
	int		id = -1;

	for (unsigned int i = 0; i < _asize; ++i)
	{
		if (!_actors[i])
		{
			id = i;
			break;
		}
	}
	if (id < 0)
	{
		_actors = resize(_actors, _asize, _asize < 1);
		_asize <<= 1;
		for (unsigned int i = _asize > 1; i < _asize; ++i)
			_actors[i] = 0;
	}
	_actors[id] = Factory::get_instance().create(this, (need_replication && !local ? ne->new_replication(id) : 0), id, name, owner);
	_actors[id]->postinstanciation();
	return (_actors[id]);
}

Actor		*Actormanager::create(Replication *r)
{
	_actors[r->id] = Factory::get_instance().create(this, r);
	_actors[r->id]->postinstanciation();
	return (_actors[r->id]);
}

Actor	*Actormanager::find_actor(int const id)
{
	return (id < (int)_asize ? _actors[id] : 0);
}

void	Actormanager::notify_owner(Actor *a, bool l)
{
	if (a->ownerid < (int)_asize && _actors[a->ownerid])
		_actors[a->ownerid]->notified_by_owned(a, l);
}

void	Actormanager::notify_owned(Actor *a, bool l)
{
	for	(unsigned int i = 0; i < _asize; ++i)
	{
		if (_actors[i]->ownerid == a->id)
			_actors[i]->notified_by_owner(a, l);
	}
}

void			Actormanager::control(int const id)
{
	Controller	*ctrl;

	if ((ctrl = (Controller *)find_actor(id)))
		ctrl->bind();
	else
		_controllermap[id] = ctrl;
}

void				Actormanager::tick(float delta)
{
	std::cout << "em" << std::endl;
	em->event();
	std::cout << "cl" << std::endl;
	cl->tick(delta);
	std::cout << "ne" << std::endl;
	if (ne)
		ne->tick(delta);
	std::cout << "pe" << std::endl;
	pe->tick(delta);
	std::cout << "ac" << std::endl;
	for (unsigned int i = 0; i < _asize; ++i)
	{
		if (_actors[i])
		{
			if (_actors[i]->destroyed)
			{
				delete _actors[i];
				_actors[i] = 0;
			}
			else
				_actors[i]->tick(delta);
		}
	}
	if (ge)
		ge->tick(delta);
	std::cout << "end" << std::endl;
}
