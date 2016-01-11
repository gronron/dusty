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

#include "new.hpp"
#include "callbackmanager.hpp"

Callbackmanager::Callbackmanager() : _cbsize(1024), _cbfree(0), _callbacks(0)
{
	_callbacks = new Callback[_cbsize];
	for (unsigned int i = 0; i < _cbsize - 1; ++i)
	{
		_callbacks[i].entity = 0;
		_callbacks[i].next = i + 1;
	}
	_callbacks[_cbsize - 1].entity = 0;
	_callbacks[_cbsize - 1].next = -1;
}

Callbackmanager::~Callbackmanager()
{
	delete [] _callbacks;
}

void	Callbackmanager::tick(float const delta)
{
	for (unsigned int i = 0; i < _cbsize; ++i)
	{
		if (_callbacks[i].entity)
		{
			if ((_callbacks[i].timer += delta) >= _callbacks[i].delta)
			{
				if ((_callbacks[i].entity->*_callbacks[i].function)() && _callbacks[i].loop)
					_callbacks[i].timer -= _callbacks[i].delta;
				else if (_callbacks[i].entity)
					_stop_callback(i);
			}
		}
	}
}

void	Callbackmanager::start_callback(int const id, Entity *entity, bool (Entity::*function)(), float const delta, bool const loop)
{
	int	cb;

	if (_cbfree < 0)
	{
		_cbfree = _cbsize;
		_cbsize <<= 1;
		_callbacks = resize(_callbacks, _cbfree, _cbsize);
		for (unsigned int i = _cbfree; i < _cbsize - 1; ++i)
		{
			_callbacks[i].entity = 0;
			_callbacks[i].next = i + 1;
		}
		_callbacks[_cbsize - 1].entity = 0;
		_callbacks[_cbsize - 1].next = -1;
	}
	
	cb = _cbfree;
	_cbfree = _callbacks[_cbfree].next;
	_callbacks[cb].next = entity->callbacks;
	entity->callbacks = cb;

	_callbacks[cb].id = id;
	_callbacks[cb].entity = entity;
	_callbacks[cb].function = function;
	_callbacks[cb].delta = delta;
	_callbacks[cb].loop = id < 0 ? false : loop;
	_callbacks[cb].timer = 0.0f;
}

bool	Callbackmanager::is_callback_started(int const id, Entity const *entity) const
{
	if (id >= 0)
		for (unsigned int i = entity->callbacks; i != -1; i = _callbacks[i].next)
			if (_callbacks[i].id == id)
				return (true);
	return (false);
}

bool	Callbackmanager::update_callback(int const id, Entity const *entity, float const delta)
{
	if (id >= 0)
	{
		for (unsigned int i = entity->callbacks; i != -1; i = _callbacks[i].next)
		{
			if (_callbacks[i].id == id)
			{
				_callbacks[i].delta = delta;
				return (true);
			}
		}
	}
	return (false);
}

bool	Callbackmanager::update_callback(int const id, Entity const *entity, bool const loop)
{
	if (id >= 0)
	{
		for (unsigned int i = entity->callbacks; i != -1; i = _callbacks[i].next)
		{
			if (_callbacks[i].id == id)
			{
				_callbacks[i].loop = loop;
				return (true);
			}
		}
	}
	return (false);
}

bool	Callbackmanager::update_callback(int const id, Entity const *entity, float const delta, bool const loop)
{
	if (id >= 0)
	{
		for (unsigned int i = entity->callbacks; i != -1; i = _callbacks[i].next)
		{
			if (_callbacks[i].id == id)
			{
				_callbacks[i].delta = delta;
				_callbacks[i].loop = loop;
				return (true);
			}
		}
	}
	return (false);
}

void	Callbackmanager::stop_allcallbacks(Entity *entity)
{
	int	cbs = entity->callbacks;

	if (cbs != -1)
	{
		int	cb;

		entity->callbacks = -1;
		for (unsigned int i = cbs; i != -1; i = _callbacks[i].next)
		{
			_callbacks[i].entity = 0;
			cb = i;
		}
		_callbacks[cb].next = _cbfree;
		_cbfree = cbs;
	}
}

void		Callbackmanager::_stop_callback(int const cb)
{
	Entity	*entity = _callbacks[cb].entity;
	
	if (entity->callbacks == cb)
		entity->callbacks = _callbacks[cb].next;
	else
	{
		for (unsigned int i = entity->callbacks; i != -1; i = _callbacks[i].next)
		{
			if (_callbacks[i].next == cb)
			{
				_callbacks[i].next = _callbacks[cb].next;
				break;
			}
		}
	}
	_callbacks[cb].entity = 0;
	_callbacks[cb].next = _cbfree;
	_cbfree = cb;
}
