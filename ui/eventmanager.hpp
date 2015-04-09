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

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <map>
#include <vector>
#include "controller.hpp"

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
