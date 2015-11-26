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

#include "math/vec_util.hpp"
#include "random/mt19937.hpp"
#include "configmanager.hpp"
#include "particlesystem.hpp"

Particlesystem::Particlesystem(Graphicengine *g, std::string const &name, Body **bd) : Animation(g), body(bd), _size(0), _particles(0), attractor(false), spawnrate(0.0f), spawntime(0.0f), time(0.0f)
{
	Df_node const	*nd = Configmanager::get_instance().get("particle.df");

	nd = nd->safe_get(name, Df_node::BLOCK, 4);
	attractor = nd->safe_get("attractor", Df_node::INT, 1)->nbr[0];
	spawnrate = nd->safe_get("spawnrate", Df_node::FLOAT, 1)->flt[0];
	spawntime = 1.0f / spawnrate;
	fade_rate = *(vec<float, 2> *)nd->safe_get("fade_rate", Df_node::FLOAT, 2)->flt;
	velocity = *(vec<float, 2> *)nd->safe_get("velocity", Df_node::FLOAT, 2)->flt;
	
	_size = spawnrate * ((fade_rate[0] + fade_rate[1]) / 2.0f);
	_particles = new Particle[_size];
	for (unsigned int i = 0; i < _size; ++i)
	{
		_particles[i].size = (float)(MT().genrand_real1());
		_particles[i].fade_rate =  (float)MT().genrand_real1(fade_rate[0], fade_rate[1]);
		if (body)
			_particles[i].position = (*body)->position;
		else
			_particles[i].position = 4.0f;
		_particles[i].velocity[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].velocity[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].velocity[2] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].velocity = vunit<float>(_particles[i].velocity) * (float)MT().genrand_real1(velocity[0], velocity[1]);

		if (attractor)
		{
			_particles[i].size = 0.0f;
			_particles[i].position += _particles[i].velocity * _particles[i].fade_rate;
			_particles[i].velocity = -_particles[i].velocity;
		}

		Aabb	aabb;
		aabb.bottom = _particles[i].position;
		aabb.top = aabb.bottom + _particles[i].size * 0.2f;
		_particles[i].index = graphic->aabbtree.add_saabb(aabb, 4);
	}
}

Particlesystem::Particlesystem(Graphicengine *g, std::string const &name, vec<float, 4> const &v) : Animation(g), body(0), _size(0), _particles(0), attractor(false), spawnrate(0.0f), spawntime(0.0f), time(0.0f)
{
	Df_node const	*nd = Configmanager::get_instance().get("particle.df");
	
	nd = nd->safe_get(name, Df_node::BLOCK, 4);
	spawnrate = 160.0f;
	spawntime = 1.0f / spawnrate;
	attractor = nd->safe_get("attractor", Df_node::INT, 1)->nbr[0];
	_size = nd->safe_get("number", Df_node::INT, 1)->nbr[0];
	fade_rate = *(vec<float, 2> *)nd->safe_get("fade_rate", Df_node::FLOAT, 2)->flt;
	velocity = *(vec<float, 2> *)nd->safe_get("velocity", Df_node::FLOAT, 2)->flt;
	position = v;

	_particles = new Particle[_size];
	for (unsigned int i = 0; i < _size; ++i)
	{
		_particles[i].size = (float)(MT().genrand_real1());
		_particles[i].fade_rate = (float)MT().genrand_real1(fade_rate[0], fade_rate[1]);
		_particles[i].position = position;
		_particles[i].velocity[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].velocity[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].velocity[2] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].velocity = vunit<float>(_particles[i].velocity) * (float)MT().genrand_real1(velocity[0], velocity[1]);

		if (attractor)
		{
			_particles[i].size = 0.0f;
			_particles[i].position += _particles[i].velocity * _particles[i].fade_rate;
			_particles[i].velocity = -_particles[i].velocity;
		}
		Aabb	aabb;
		aabb.bottom = _particles[i].position;
		aabb.top = aabb.bottom + _particles[i].size * 0.2f;
		_particles[i].index = graphic->aabbtree.add_saabb(aabb, 3);
	}
}

Particlesystem::~Particlesystem()
{
	if (_particles)
	{
		for (unsigned int i = 0; i < _size; ++i)
			graphic->aabbtree.remove_aabb(_particles[i].index);
		delete [] _particles;
	}
}

bool	Particlesystem::tick(float delta)
{
	bool	empty = true;

	for (unsigned int i = 0; i < _size; ++i)
	{
		_particles[i].position += _particles[i].velocity * delta;
		if (attractor)
			_particles[i].size += delta / _particles[i].fade_rate;
		else
			_particles[i].size -= delta / _particles[i].fade_rate;
		if ((_particles[i].size <= 0.0f || _particles[i].size >= 1.0f) && spawnrate != 0.0f)
		{
			_particles[i].size = 1.0f;
			_particles[i].fade_rate =  (float)MT().genrand_real1(fade_rate[0], fade_rate[1]);
			if (body)
				_particles[i].position = (*body)->position;
			else
				_particles[i].position = position;
			_particles[i].velocity[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			_particles[i].velocity[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			_particles[i].velocity[2] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			_particles[i].velocity = vunit<float>(_particles[i].velocity) * (float)MT().genrand_real1(velocity[0], velocity[1]);

			if (attractor)
			{
				_particles[i].size = 0.0f;
				_particles[i].position += _particles[i].velocity * _particles[i].fade_rate;
				_particles[i].velocity = -_particles[i].velocity;
			}
		}
		if (_particles[i].size > 0.0f)
		{
			empty = false;
			Aabb	aabb;
			aabb.bottom = _particles[i].position;
			aabb.top = aabb.bottom + _particles[i].size * 0.25f;
			graphic->aabbtree.move_saabb(_particles[i].index, aabb);
		}
	}
	if (spawnrate == 0.0f && empty)
		return (false);
	else
		return (true);
}

void	Particlesystem::stop()
{
	spawnrate = 0.0f;
}

void	Particlesystem::reverse()
{
	attractor = !attractor;
	for (unsigned int i = 0; i < _size; ++i)
		_particles[i].velocity = -_particles[i].velocity;
}
