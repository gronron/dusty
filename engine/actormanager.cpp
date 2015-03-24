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
#include "uimanager.hpp"
#include "actormanager.hpp"

#include <iostream>

Actormanager::Actormanager(Actormanager::Option const &opt) : cl(0), em(0), pe(0), ne(0), ge(0), um(0), master(opt.master), local(opt.local), graphic(opt.graphic), audio(opt.audio), controllerclass(), _currentid(0)
{
	Df_node	*nd;

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
		um = new Uimanager();
	}
	controllerclass = nd->safe_get("controller", Df_node::STRING, 1)->cstr[0];

	if (master)
		create(opt.level, 0);
	if (local)
	{
		Controller *ctrl = (Controller *)create(controllerclass, 0);
		_controllermap[ctrl->id] = ctrl;
		ctrl->bind();
	}
}

Actormanager::~Actormanager()
{
	Actoriterator	i;

	for (i = _actormap.begin(); i != _actormap.end(); ++i)
		delete i->second;
	if (graphic)
	{
		delete um;
		delete ge;
	}
	if (ne)
		delete ne;
	delete pe;
	delete em;
	delete cl;
}

Actor		*Actormanager::create(std::string const &name, Actor const *owner)
{
	Actor	*actr;
	int		id;

	id = ++_currentid;
	actr = Factory::get_instance().create(this, name, id, owner);
	_actormap[id] = actr;
	actr->postinstanciation();
	return (actr);
}

Actor		*Actormanager::create(Replication *ri)
{
	Actor	*actr;

	actr = Factory::get_instance().create(this, ri->type, ri);
	_actormap[ri->id] = actr;
	return (actr);
}

Actor				*Actormanager::findactor(int id)
{
	Actoriterator	i;

	i = _actormap.find(id);
	return (i == _actormap.end() ? 0 : i->second);
}

void				Actormanager::notify_owner(Actor *a, bool l)
{
	Actoriterator	i;

	if ((i = _actormap.find(a->ownerid)) != _actormap.end())
		i->second->notified_by_owned(a, l);
}

void				Actormanager::notify_owned(Actor *a, bool l)
{
	Actoriterator	i;

	for (i = _actormap.begin(); i != _actormap.end(); ++i)
	{
		if (i->second->ownerid == a->id)
			i->second->notified_by_owner(a, l);
	}
}

void			Actormanager::control(int id)
{
	Controller	*ctrl;

	if ((ctrl = (Controller *)findactor(id)))
		ctrl->bind();
	else
		_controllermap[id] = ctrl;
}

void				Actormanager::tick(float delta)
{
	Actoriterator	i;

	//std::cout << "em" << std::endl;
	em->event();
	//std::cout << "cl" << std::endl;
	cl->tick(delta);
	//std::cout << "ne" << std::endl;
	if (ne)
		ne->tick(delta);
	//std::cout << "pe" << std::endl;
	pe->tick(delta);
	//std::cout << "ac" << std::endl;
	for (i = _actormap.begin(); i != _actormap.end();)
	{
		if (i->second->destroyed)
		{
			if (!i->second->cleared)
			{
				delete i->second;
				_actormap.erase(i++);
			}
		}
		else
		{
			i->second->tick(delta);
			++i;
		}
	}
	//std::cout << "end" << std::endl;
}
