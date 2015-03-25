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

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <map>
#include <vector>
#include "controller.hpp"

namespace	sf
{
	class	Event;
}

class	Actormanager;

class	Eventmanager
{
	public:

		struct	Bind
		{
			Bind() : name(), ctrl(0), fx(0), size(0) { }

			std::string				name;
			Controller				*ctrl;
			Controller::BINDTYPE	fx;
			int						size;
		};


		Actormanager				*am;

		std::map<int, Controller *>	_controllermap;

		bool						running;
		bool						typing;
		Bind						_mousemove;
		std::vector<Bind>			_keyvector;
		std::vector<Bind>			_mousebuttonvector;
		std::vector<Bind>			_joybuttonvector;
		std::vector<Bind>			_joymovevector;


		Eventmanager(Actormanager *);
		~Eventmanager();

		bool	bind(std::string const &, Controller *, Controller::BINDTYPE);
		void	toogletyping();

		void	event();

		void	_key(sf::Event &, float );
		void	_mousebutton(sf::Event &, float);
		void	_joybutton(sf::Event &, float);
		void	_joymove(sf::Event &);
		void	_bidon(sf::Event &);
};

#endif
