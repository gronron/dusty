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

Particlesystem::Particlesystem(Graphicengine *ge, float d, std::string const &name, Boundingbox **bdb) : Animation(ge, d), boundingbox(bdb), _size(0), _particles(0), attractor(false), spawnrate(0.0f), spawntime(0.0f), time(0.0f)
{
	Df_node const	*nd = Configmanager::get_instance().get("particle.df");

	nd = nd->safe_get(name, Df_node::BLOCK, 6);
	attractor = nd->safe_get("attractor", Df_node::INT, 1)->nbr[0];
	spawnrate = nd->safe_get("spawnrate", Df_node::FLOAT, 1)->flt[0];
	spawntime = 1.0f / spawnrate;
	cmin = *(vec<float, 3> *)nd->safe_get("cmin", Df_node::FLOAT, 3)->flt;
	cmax = *(vec<float, 3> *)nd->safe_get("cmax", Df_node::FLOAT, 3)->flt;
	fade = *(vec<float, 2> *)nd->safe_get("fade", Df_node::FLOAT, 2)->flt;
	speed = *(vec<float, 2> *)nd->safe_get("speed", Df_node::FLOAT, 2)->flt;
	
	_size = spawnrate * ((fade[0] + fade[1]) / 2.0);
	_particles = new Particle[_size];
	for (unsigned int i = 0; i < _size; ++i)
		_particles[i].fade = 0.0f;
	tex = ge->load_texture("particle.png");
}

Particlesystem::Particlesystem(Graphicengine *ge, float d, std::string const &name, vec<float, 2> const &v) : Animation(ge, d), boundingbox(0), _size(0), _particles(0), attractor(false), spawnrate(0.0f), spawntime(0.0f), time(0.0f)
{
	Df_node const	*nd = Configmanager::get_instance().get("particle.df");
	
	nd = nd->safe_get(name, Df_node::BLOCK, 6);
	spawnrate = 0.0f;
	spawntime = 0.0f;
	attractor = nd->safe_get("attractor", Df_node::INT, 1)->nbr[0];
	_size = nd->safe_get("number", Df_node::INT, 1)->nbr[0];
	cmin = *(vec<float, 3> *)nd->safe_get("cmin", Df_node::FLOAT, 3)->flt;
	cmax = *(vec<float, 3> *)nd->safe_get("cmax", Df_node::FLOAT, 3)->flt;
	fade = *(vec<float, 2> *)nd->safe_get("fade", Df_node::FLOAT, 2)->flt;
	speed = *(vec<float, 2> *)nd->safe_get("speed", Df_node::FLOAT, 2)->flt;

	_particles = new Particle[_size];
	for (unsigned int i = 0; i < _size; ++i)
	{
		_particles[i].color[0] = (float)MT().genrand_real1(cmin[0], cmax[0]);
		_particles[i].color[1] = (float)MT().genrand_real1(cmin[1], cmax[1]);
		_particles[i].color[2] = (float)MT().genrand_real1(cmin[2], cmax[2]);
		_particles[i].color[3] = 1.0f;

		_particles[i].fade = (float)MT().genrand_real1(fade[0], fade[1]);
		_particles[i].loc = v;
		_particles[i].spd[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].spd[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		_particles[i].spd = Sgl::unit(_particles[i].spd) * (float)MT().genrand_real1(speed[0], speed[1]);

		if (attractor)
		{
			_particles[i].loc += _particles[i].spd * _particles[i].fade;
			_particles[i].spd = -_particles[i].spd;
			_particles[i].color[3] = 0.0f;
		}
	}
	tex = ge->load_texture("particle.png");
}

Particlesystem::~Particlesystem()
{
	if (_particles)
		delete [] _particles;
}

bool	Particlesystem::tick(float delta)
{
	bool	empty = true;

	for (unsigned int i = 0; i < _size; ++i)
	{
		_particles[i].loc += _particles[i].spd * delta;
		if (attractor)
		{
			_particles[i].color[3] += delta / _particles[i].fade;
			if (_particles[i].color[3] > 1.0f)
				_particles[i].fade = 0.0f;
		}
		else
		{
			_particles[i].color[3] -= delta / _particles[i].fade;
			if (_particles[i].color[3] <= 0.0f)
				_particles[i].fade = 0.0f;
		}
		if (_particles[i].fade == 0.0f && spawnrate != 0.0f)
		{
			_particles[i].color[0] = (float)MT().genrand_real1(cmin[0], cmax[0]);
			_particles[i].color[1] = (float)MT().genrand_real1(cmin[1], cmax[1]);
			_particles[i].color[2] = (float)MT().genrand_real1(cmin[2], cmax[2]);
			_particles[i].color[3] = 1.0f;

			_particles[i].fade =  (float)MT().genrand_real1(fade[0], fade[1]);
			_particles[i].loc = (*boundingbox)->loc;
			_particles[i].spd[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			_particles[i].spd[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			_particles[i].spd = Sgl::unit(_particles[i].spd) * (float)MT().genrand_real1(speed[0], speed[1]);

			if (attractor)
			{
				_particles[i].loc += _particles[i].spd * _particles[i].fade;
				_particles[i].spd = -_particles[i].spd;
				_particles[i].color[3] = 0.0f;
			}
		}
		if (_particles[i].fade != 0.0f)
			empty = false;
	}
	if (spawnrate == 0.0f && empty)
		return (false);
	else
		return (true);
}

void				Particlesystem::draw()
{
	vec<float, 2>	s;
	s = 0.075f;

	for (unsigned int i = 0; i < _size; ++i)
		_ge->draw_texture(tex, _particles[i].loc, s, _particles[i].color);
}

void	Particlesystem::stop()
{
	spawnrate = 0.0f;
}

void	Particlesystem::reverse()
{
	attractor = !attractor;
	for (unsigned int i = 0; i < _size; ++i)
		_particles[i].spd = -_particles[i].spd;
}
