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

#include "configmanager.hpp"
#include "effectfactory.hpp"
#include "particlesystem.hpp"

Particlesystem::Particlesystem(Graphicengine *g, std::string const &name, Body **bd) : Animation(g), running(true), body(bd), _prtclssize(0), _particles(0)
{
	DFNode const	*nd = Configmanager::get_instance().get("particle")->safe_get(name, DFNode::BLOCK, 1);

	material = *nd->safe_get("material", DFNode::INT, 1)->nbr;
	scale = *nd->safe_get("scale", DFNode::FLOAT, 1)->flt;
	_prtclssize = *nd->safe_get("number", DFNode::INT, 1)->nbr;
	_particles = new Particle[_prtclssize];
	position = (*body)->position;

	for (unsigned int i = 0; i < 8; ++i)
		_effects[i].pe = 0;

	for (unsigned int i = 0; i < _prtclssize; ++i)
		_particles[i].active = false;

	add_effects(nd->safe_get("effects", DFNode::BLOCK, 1));
}

Particlesystem::Particlesystem(Graphicengine *g, std::string const &name, vec<float, 4> const &v) : Animation(g), running(true), body(0), _prtclssize(0), _particles(0)
{
	DFNode const	*nd = Configmanager::get_instance().get("particle")->safe_get(name, DFNode::BLOCK, 1);

	material = *nd->safe_get("material", DFNode::INT, 1)->nbr;
	scale = *nd->safe_get("scale", DFNode::FLOAT, 1)->flt;
	_prtclssize = *nd->safe_get("number", DFNode::INT, 1)->nbr;
	_particles = new Particle[_prtclssize];
	position = v;

	for (unsigned int i = 0; i < 8; ++i)
		_effects[i].pe = 0;

	for (unsigned int i = 0; i < _prtclssize; ++i)
		_particles[i].active = false;

	add_effects(nd->safe_get("effects", DFNode::BLOCK, 1));
}

Particlesystem::Particlesystem(Graphicengine *g, std::string const &name, vec<float, 4> const &v, unsigned int s, Particle *p) : Animation(g), running(true), position(v), body(0), material(0), scale(1.0f), _prtclssize(s), _particles(p)
{
	DFNode const	*nd = Configmanager::get_instance().get("particle")->safe_get(name, DFNode::BLOCK, 1);

	for (unsigned int i = 0; i < 8; ++i)
		_effects[i].pe = 0;

	for (unsigned int i = 0; i < _prtclssize; ++i)
		_particles[i].active = false;

	add_effects(nd->safe_get("effects", DFNode::BLOCK, 1));
}

Particlesystem::~Particlesystem()
{
	delete [] _particles;
}

void	Particlesystem::stop()
{
	running = false;
}

void	Particlesystem::attach(Body **bd)
{
	body = bd;
}

void	Particlesystem::detach()
{
	body = 0;
}

bool		Particlesystem::tick(float const delta)
{
	bool	alive = false;

	if (body)
		position = (*body)->position;
	for (unsigned int i = 0; i < 8; ++i)
	{
		if (_effects[i].pe)
		{
			alive = true;
			_effects[i].pe->tick(delta);
			if (_effects[i].pe->timer >= 0.0f && (_effects[i].pe->timer -= delta) <= 0.0f)
			{
				delete _effects[i].pe;
				_effects[i].pe = 0;
				if (_effects[i].next)
					add_effects(_effects[i].next);
			}
		}
	}

	for (unsigned int i = 0; i < _prtclssize; ++i)
	{
		if (_particles[i].active)
		{
			Aabb	aabb;
			aabb.bottom = _particles[i].position;
			aabb.top = aabb.bottom + _particles[i].size * scale;
			graphic->add_dynamic_block(aabb, 1, 5);
		}
	}
	return (alive);
}

void	Particlesystem::add_effects(DFNode const *nd)
{
	for (unsigned int i = 0; i < nd->size; ++i)
	{
		unsigned int	idx;
		for (idx = 0; idx < 8; ++idx)
			if (!_effects[idx].pe)
				break;

		float const		timer = *nd->node[i]->safe_get("timer", DFNode::FLOAT, 1)->flt;
		DFNode const	*data = nd->node[i]->get("data");

		if (data->type == DFNode::REFERENCE)
			data = data->node[0];
		_effects[idx].pe = Effectfactory::get_instance().create(crc32(nd->node[i]->name.c_str()), this, timer, data);
		_effects[idx].next = data->get("effects");
	}
}
