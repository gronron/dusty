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

#ifndef CALLBACKMANAGER_H_
#define CALLBACKMANAGER_H_

#include "actor.hpp"

class	Callbackmanager
{
	public:

		struct	Callback
		{
			int			id;
			Actor		*actor;
			bool		(Actor::*function)();
			float		delta;
			float		timer;
			bool		loop;
			int			next;
		};


		unsigned int	_cbsize;
		int				_cbfree;
		Callback		*_callbacks;


		Callbackmanager();
		~Callbackmanager();

		void	tick(float const delta);

		void	start_callback(int const id, Actor *actor, bool (Actor::*)(), float const delta, bool const loop);
		void	stop_callback(int const id, Actor *actor);

		bool	is_callback_started(int const id, Actor const *actor) const;
		bool	update_callback(int const id, Actor const *actor, float const delta);
		bool	update_callback(int const id, Actor const *actor, bool const loop);
		bool	update_callback(int const id, Actor const *actor, float const delta, bool const loop);

		void	stop_allcallbacks(Actor *actor);

		void	_stop_callback(int const callback);
};

#endif
