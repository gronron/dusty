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
#include "math/vec_util.hpp"
#include "actor.hpp"
#include "physicengine.hpp"
#include <iostream>
#include <cstdlib>

Physicengine::Physicengine() : _bsize(64), _bodies(0), _bfree(0), _pcount(0), _psize(64), _pairs(0), _currentquery(0)
{
	_bodies = new Body[_bsize];
	_pairs = new Pair[_psize];

	for (unsigned int i = 0; i < _bsize - 1; ++i)
			_bodies[i].next = i + 1;
	_bodies[_bsize - 1].next = -1;
}

Physicengine::~Physicengine()
{
	delete [] _bodies;
}

void		Physicengine::new_body(Body **link)
{
	Body	*body;

	if (_bfree == -1)
	{
		_bfree = _bsize;
		_bodies = resize(_bodies, _bsize,_bsize << 1);
		_bsize <<= 1;
		for (unsigned int i = 0; i < _bsize >> 1; ++i)
			*_bodies[i].link = _bodies + i;
		for (unsigned int i = _bsize >> 1; i < _bsize - 1; ++i)
			_bodies[i].next = i + 1;
		_bodies[_bsize - 1].next = -1;	
	}

	body = _bodies + _bfree;
	_bfree = _bodies[_bfree].next;
	
	body->link = link;
	*body->link = body;
	body->index = -1;
}

void		Physicengine::init_body(Body *body)
{
	Aabb	aabb;

	body->shape->compute_aabb(aabb, body->position);
	if (body->dynamic)
		body->index = _dynamictree.add_aabb(aabb, (unsigned int)(body - _bodies));
	else
		body->index = _statictree.add_aabb(aabb, (unsigned int)(body - _bodies));
}
/*
void	Physicengine::move(Body *body, vec<float, 4> const &position)
{

	//_statictree.move_aabb(index, aabb);
}	
*/
void		Physicengine::delete_body(Body *body)
{
	if (body->index != -1)
	{
		if (body->dynamic)
			_dynamictree.remove_aabb(body->index);
		else
			_statictree.remove_aabb(body->index);
		body->index = -1;
	}
	body->next = _bfree;
	_bfree = body - _bodies;
}

void		Physicengine::tick(float delta)
{
	for (unsigned int i = 0; i < _bsize; ++i)
	{
		if (_bodies[i].index != -1 && _bodies[i].dynamic)
		{
			_bodies[i].prevposition = _bodies[i].position;
			_bodies[i].prevvelocity = _bodies[i].velocity;
			_bodies[i].position = _bodies[i].position + _bodies[i].velocity * delta + _bodies[i].acceleration * delta * delta * 0.5f; // + bd->ping
			_bodies[i].velocity = _bodies[i].velocity + _bodies[i].acceleration * delta;

			_bodies[i].shape->compute_aabb(_bodies[i].aabb, _bodies[i].position);
			_dynamictree.move_aabb(_bodies[i].index, _bodies[i].aabb);
		}
	}
	/*
	_pcount = 0;
	for (unsigned int i = 0; i < _bsize; ++i)
	{
		if (_bodies[i].index != -1)
		{
			_currentquery = i;
			_dynamictree.query(_bodies[i].aabb, this, &Physicengine::_add_pair);
			_statictree.query(_bodies[i].aabb, this, &Physicengine::_add_pair);
		}
	}
	for (unsigned int i = 0; i < _pcount; ++i)
	{
		//_bodies[_pairs[i].a].collider->collide(_bodies[_pairs[i].b].collider);
		//_bodies[_pairs[i].b].collider->collide(_bodies[_pairs[i].a].collider);
	}*/
}

void	Physicengine::_add_pair(int const index)
{
	if (index > _currentquery || !_bodies[index].dynamic)
	{
		if (_pcount >= _psize)
		{
			_psize <<= 1;
			_pairs = resize(_pairs, _pcount, _psize);
		}
	
		_pairs[_pcount].a = _currentquery;
		_pairs[_pcount].b = index;
		++_pcount;
	}
}
/*
void				Physicengine::_collide2(float delta, Boundingbox *x, Boundingbox *y)
{
	vec<float, 3>	u;
	vec<float, 3>	v;
	
	vec<float, 3>	d;
	vec<float, 3>	e;
	
	d = 2.0f * (x->acc - y->acc) * (x->loc - (y->loc + y->size));
	if (d >= 0.0f)
	{
	
	}
	else
	{
		d = 2.0f * (y->acc - x->acc) * (y->loc - (x->loc + x->size));
	}
	
	u = (-x->spd + sqrt(d)) / (2.0f * x->acc);
	v = (-x->spd - sqrt(d)) / (2.0f * x->acc);

	for (unsigned int i = 0; i < 3; ++i)
	{
		float	t = -1.0f;

		if (d[i] > 0.0f)
		{
		
		}
		if (u[i] >= 0.0f && u[i] <= delta)
			t = v[i] >= 0.0f && v[i] < u[i] ? v[i] : u[i];
		else if (v[i] >= 0.0f && v[i] <= delta)
			t = v[i];
		if (t >= 0.0f && _reaction(x, y, t, i))
			break;
	}
}

void				Physicengine::_collide(float delta, Boundingbox *x, Boundingbox *y)
{
	vec<float, 2>	u;
	vec<float, 2>	v;

	//std::cout << "collide begin" << std::endl;
	u = (y->loc - (x->loc + x->size)) / (x->spd - y->spd);
	v = (x->loc - (y->loc + y->size)) / (y->spd - x->spd);
	for (unsigned int i = 0; i < 2; ++i)
	{
		float t = -1.0f;
		if (u[i] >= 0.0f && u[i] <= delta)
			t = v[i] >= 0.0f && v[i] < u[i] ? v[i] : u[i];
		else if (v[i] >= 0.0f && v[i] <= delta)
			t = v[i];
		if (t >= 0.0f && _reaction(x, y, t, i))
			break;
	}
}

bool				Physicengine::_reaction(Boundingbox *x, Boundingbox *y, float t, unsigned int axis)
{
	vec<float, 2>	xloc;
	vec<float, 2>	yloc;

	//std::cout << "reaction begin" << std::endl;
	xloc = x->loc + x->spd * t;
	yloc = y->loc + y->spd * t;
	for (unsigned int i = 0; i < 2; ++i)
	{
		if (i != axis && (xloc[i] > yloc[i] + y->size[i] || yloc[i] > xloc[i] + x->size[i]))
			return (false);
	}
	//std::cout << "reaction" << std::endl;
	if (x->actor && x->actor->collide(*y->actor))
		x->nextloc[axis] = xloc[axis];
	if (y->actor && y->actor->collide(*x->actor))
		y->nextloc[axis] = yloc[axis];
	//std::cout << "reaction ended" << std::endl;
	return (true);
}*/
