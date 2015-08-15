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

#include "callbackmanager.hpp"

Callbackmanager::Callbackmanager() : _dbsize(1024), _dbfree(0), _callbacks(0)
{
	_callbacks = new Callback[_cbsize];
	for (unsigned int i = 0; i < _cbsize - 1; ++i)
		_callbacks[i].next = i + 1;
	_callbacks[_cbsize - 1] = -1;
}

Callbackmanager::~Callbackmanager()
{
	delete [] _callbacks;
}

void	Callbackmanager::tick(float const delta)
{
	for (unsigned int i = 0; i < _cbsize; ++i)
	{
		if (_callbacks[i].actor)
		{
			if (_callbacks[i].timer += delta) >= _callbacks[i].delta)
			{
				if (_callbacks[i].actor->(_callbacks[i].function)() && _callbacks[i].loop)
					_callbacks[i].timer -= _callbacks[i].delta;
				else
					stop_callback(i);
			}
		}
	}
}

void	Callbackmanager::start_callback(Actor *actor, bool (Actor::*fx)(), float const delta, bool const loop)
{
	int	callback;
	
	if (_cbfree == -1)
	{
		_cbfree = _cbsize;
		_callbacks = resize(_callbacks, _cbsize, _cbsize << 1);
		for (unsigned int i = _cbsize; i < _cbsize - 1; ++i)
			_callbacks[i].next = i + 1;
		_callbacks[_cbsize].next = -1;
	}
	
	callback = _cbfree;
	_cbfree = _callbacks[_cbfree].next;
	_callbacks[callback].next = actor->callbacks;
	actor->callbacks = callback;

	_callbacks[callback].actor = actor;
	_callbacks[callback].function = function;
	_callbacks[callback].delta = delta;
	_callbacks[callback].loop = loop;
	_callbacks[callback].timer = 0.0f;
}

void		Callbackmanager::stop_callback(int const callback)
{
	Actor	*actor = _callback[callback].actor;
	
	if (actor->callbacks == callback)
		actor->callbacks = _callbacks[callback].next;
	else
	{
		for (unsigned int i = actor->callbacks; i != -1; i = _callbacks[i].next)
		{
			if (_callbacks[i].next == callback)
			{
				_callbacks[i].next = _callbacks[callback].next;
				break;
			}
		}
	}
	_callbacks[callback].actor = 0;
	_callbacks[callback].next = _cbfree;
	_cbfree = callback;
}

/*
void	Actor::update_callback(std::string const &name, float delta)
{
	std::map<std::string, Callback>::iterator	i;
	
	if ((i = _callbackmap.find(name)) != _callbackmap.end())
		i->second.delta = delta;
}

bool	Actor::is_callback_started(std::string const &name)
{
	return (_callbackmap.find(name) != _callbackmap.end());
}
*/
