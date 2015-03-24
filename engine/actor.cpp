#include <sstream>
#include "replication.hpp"
#include "endian/packet.hpp"
#include "actor.hpp"

Actor::Actor(Actormanager *a, Replication *r, short int t, int i, Actor const *o) : _callbackid(0), am(a), rp(r), type(t), id(r ? r->id : i), ownerid(o ? o->id : 0), destroyed(false), cleared(true), ping(0.0f)
{

}

Actor::~Actor()
{

}

void	Actor::postinstanciation()
{

}

void	Actor::destroy()
{
    destroyed = true;
	if (rp)
		rp->destroy();
}

void	Actor::notified_by_owner(Actor *, bool)
{

}

void	Actor::notified_by_owned(Actor *, bool)
{

}

void	Actor::get_replication(Packet &pckt)
{
	pckt.write(ownerid);
}

void	Actor::replicate(Packet &pckt, float p)
{
	ping = p;
	pckt.read(ownerid);
}

void											Actor::tick(float delta)
{
	std::map<std::string, Callback>::iterator	i;

	ping = 0.0f;
	for (i = _callbackmap.begin(); i != _callbackmap.end();)
	{
		if ((i->second.timer += delta) >= i->second.delta)
		{
			if ((this->*i->second.fx)() && i->second.loop)
			{
				i->second.timer -= i->second.delta;
				++i;
			}
			else
				_callbackmap.erase(i++);
		}
		else
			++i;
	}
}

bool	Actor::collide(Actor const &)
{
	return (false);
}

void			Actor::start_callback(std::string const &name, float delta, bool loop, bool (Actor::*fx)())
{
	Callback	cb;

	cb.delta = delta;
	cb.timer = 0.0f;
	cb.loop = loop;
	cb.fx = fx;
	_callbackmap[name] = cb;
}

void					Actor::start_callback(float delta, bool (Actor::*fx)())
{
	Callback			cb;
	std::ostringstream	name;

	name << '_' << _callbackid++ << '_';
	cb.delta = delta;
	cb.timer = 0.0f;
	cb.loop = false;
	cb.fx = fx;
	_callbackmap[name.str()] = cb;
}

void	Actor::stop_callback(std::string const &name)
{
	_callbackmap.erase(name);
}

void	Actor::update_callback(std::string const &name, float delta)
{
	std::map<std::string, Callback>::iterator	i;
	
	if ((i = _callbackmap.find(name)) != _callbackmap.end())
		i->second.delta = delta;
}

bool	Actor::is_callback_started(std::string const &name)
{
	return (_callbackmap.find(name) != _callbackmap.end());
}
