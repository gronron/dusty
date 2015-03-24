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

#ifndef ACTORMANAGER_H_
#define ACTORMANAGER_H_

#include <map>
#include <string>

class	Actor;
class	Replication;
class	Controller;
class	Console;
class	Eventmanager;
class	Uimanager;
class	Physicengine;
class	Networkengine;
class	Graphicengine;

class	Actormanager
{
	public:

		struct	Option
		{
			bool		master;
			bool		local;
			bool		graphic;
			bool		audio;
			std::string	ip;
			std::string	port;
			std::string	level;
		};

		typedef	std::map<int, Actor *>::iterator	Actoriterator;


		Console					*cl;
		Physicengine			*pe;
		Networkengine			*ne;
		Graphicengine			*ge;
		Eventmanager			*em;
		Uimanager				*um;

		bool const				master;
		bool const				local;
		bool const				graphic;
		bool const				audio;

		std::string				controllerclass;

		int							_currentid;
		std::map<int, Actor *>		_actormap;
		std::map<int, Controller *>	_controllermap;


		Actormanager(Option	const &);
		~Actormanager();

		Actor	*create(std::string const &, Actor const *);
		Actor	*create(Replication *);
		Actor	*findactor(int);
		void	notify_owner(Actor *, bool);
		void	notify_owned(Actor *, bool);
		void	destroy(int);
		void	control(int);

		void	tick(float);
};

#endif
