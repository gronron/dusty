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

#include <sstream>
#include "replication.hpp"
#include "endian/packet.hpp"
#include "actor.hpp"

Actor::Actor(Actormanager *a, Replication *r, int i, short int t, Actor const *o) : _callbackid(0), am(a), rp(r), type(t), id(r ? r->id : i), ownerid(o ? o->id : 0), state(CREATED), ping(0.0f)
{

}

Actor::~Actor()
{

}

void	Actor::postinstanciation()
{
	state = OK;
}

void	Actor::destroy()
{
    state = DESTROYED;
	if (rp)
		rp->destroy();
}

void	Actor::notified_by_owner(Actor *, bool)
{

}

void	Actor::notified_by_owned(Actor *, bool)
{

}

void	Actor::get_replication(Packet &pckt) const
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
