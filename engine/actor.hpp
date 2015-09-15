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

#ifndef ACTOR_H_
#define ACTOR_H_

#include "endian/packet.hpp"
#include "collider.hpp"

class	Gameengine;
class	Replication;

class	Actor : public Collider
{
	public:
	
		enum	State { CREATED, OK, DESTROYED };


		Gameengine		*engine;
		Replication		*rp;
		int				callbacks;

		short int	type;
		int			id;
		int			ownerid;
		char		state;
		float		ping;


		Actor(Gameengine *, Replication *, int const, short int const, Actor const *);
		virtual ~Actor();

		virtual void	postinstanciation();
		virtual void	destroy();

		virtual void	notified_by_owner(Actor *, bool const);
		virtual void	notified_by_owned(Actor *, bool const);

		virtual void	get_replication(Packet &) const;
        virtual void	replicate(Packet &, float const);

		virtual void	tick(float const);
		virtual bool	collide(Collider *);
};

#endif
