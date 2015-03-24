///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2012 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of Chasis.
//
//    Chasis is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Chasis is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Chasis.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

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
		bool		destroyed;
		bool		cleared;
		float		ping;


		Actor(Actormanager *, Replication *, short int, int, Actor const *);
		virtual ~Actor();

		virtual void	postinstanciation();
		virtual void	destroy();

		virtual void	notified_by_owner(Actor *, bool);
		virtual void	notified_by_owned(Actor *, bool);

		virtual void	get_replication(Packet &);
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
