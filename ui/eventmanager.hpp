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

#pragma once

#include <map>
#include <vector>
#include <SDL/SDL_events.h>
#include "controller.hpp"

class	Gameengine;

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


		Gameengine				*engine;

		std::map<int, Controller *>	_controllermap;

		bool	running;
		bool	typing;
		Bind	_keys[SDL_NUM_SCANCODES];
		Bind	_mousemove;
		Bind	_mousebuttons[8];
		Bind	_mousewheel;


		Bind	_gamepadbuttons[32];
		Bind	_gamepadmoves[8];


		Eventmanager(Gameengine *);
		~Eventmanager();

		void	event();

		void	_type(SDL_Event &);
		void	_key(SDL_Event &, float );
		void	_mousebutton(SDL_Event &, float);
		void	_gamepadbutton(SDL_Event &, float);
		void	_gamepadmove(SDL_Event &);
};
