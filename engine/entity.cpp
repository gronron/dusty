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

#include "endian/packet.hpp"
#include "replication.hpp"
#include "callbackmanager.hpp"
#include "gameengine.hpp"
#include "entity.hpp"

Entity::Entity(Gameengine *g, Replication *r, int const i, short int const t, Entity const *o) : engine(g), rp(r), callbacks(-1), type(t), id(r ? r->id : i), ownerid(o ? o->id : 0), state(CREATED), ping(0.0f)
{

}

Entity::~Entity()
{

}

void	Entity::postinstanciation()
{
	state = OK;
}

void	Entity::destroy()
{
    state = DESTROYED;
	if (rp)
		rp->destroy();
	engine->callback->stop_allcallbacks(this);
}

void	Entity::notified_by_owner(Entity *, bool const)
{

}

void	Entity::notified_by_owned(Entity *, bool const)
{

}

void	Entity::get_replication(Packet &pckt) const
{
	pckt.write(ownerid);
}

void	Entity::replicate(Packet &pckt, float const p)
{
	ping = p;
	pckt.read(ownerid);
}

void	Entity::tick(float const)
{
	ping = 0.0f;
}

bool	Entity::should_collide(Collider const *) const
{
	return (false);
}

bool	Entity::collide(Collider *)
{
	return (false);
}
