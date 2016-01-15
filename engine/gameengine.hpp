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

#ifndef GAMEENGINE_H_
#define GAMEENGINE_H_

#include <map>
#include <string>

class	Entity;
class	Replication;
class	Controller;
class	Console;
class	Eventmanager;
//class	Uimanager;
class	Physicengine;
class	Networkengine;
class	Callbackmanager;
class	Graphicengine;

class	Gameengine
{
	public:

		struct			Option
		{
			bool		master;
			bool		local;
			bool		graphic;
			bool		audio;
			std::string	ip;
			std::string	port;
			std::string	level;
		};


		bool			master;
		
		unsigned int	_entsize;
		Entity			**_entities;

		Callbackmanager	*callback;
		Physicengine	*physic;
		Networkengine	*network;

		Console			*console;
		Graphicengine	*graphic;
		Eventmanager	*event;
		//Uimanager		*um;

		std::string					controllerclass;


		Gameengine(Option const &);
		~Gameengine();

		Entity	*create(std::string const &, Entity const *, bool const need_replication);
		Entity	*create(Replication *);
		Entity	*find_entity(int const);
		void	notify_owner(Entity *, bool const);
		void	notify_owned(Entity *, bool const);
		void	destroy(int const);
		void	control(int const);

		void	tick(float const);
};

#endif
