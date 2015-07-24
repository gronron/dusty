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

#include <string>
#include <map>

class	Actormanager;
class	Replication;
class	Packet;

class	Actor
{
	public:
	
		enum	State { CREATED, OK, DESTROYED };

		struct	Callback
		{
			float	delta;
			float	timer;
			bool	loop;
			bool	(Actor::*fx)();
		};


		int								_callbackid;
		std::map<std::string, Callback>	_callbackmap;

		Actormanager	*am;
		Replication		*rp;

		short int	type;
		int			id;
		int			ownerid;
		char		state;
		float		ping;


		Actor(Actormanager *, Replication *, int, short int, Actor const *);
		virtual ~Actor();

		virtual void	postinstanciation();
		virtual void	destroy();

		virtual void	notified_by_owner(Actor *, bool);
		virtual void	notified_by_owned(Actor *, bool);

		virtual void	get_replication(Packet &) const;
        virtual void	replicate(Packet &, float);

		virtual void	tick(float);
		virtual bool	collide(Actor const &);

		virtual void	start_callback(std::string const &, float, bool, bool (Actor::*)());
		virtual void	start_callback(float, bool (Actor::*)());
		virtual void	stop_callback(std::string const &);
		virtual void	update_callback(std::string const &, float);
		virtual bool	is_callback_started(std::string const &);
};

#endif
