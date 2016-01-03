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

#include <cfloat>
#include "new.hpp"
#include "math/vec_util.hpp"
#include "thread/lightthreadpool.hpp"
#include "thread/lightthreadpool_util.hpp"
#include "shape.hpp"
#include "collider.hpp"
#include "narrowphase.hpp"
#include "physicengine.hpp"
#include <iostream>
#include <cstdlib>

Physicengine::Physicengine() : _bdsize(1024), _bdfree(0), _bodies(0), _prcount(0), _prsize(1024), _pairs(0), _currentquery(0)
{
	_bodies = new Body[_bdsize];
	_pairs = new Pair[_prsize];

	for (unsigned int i = 0; i < _bdsize - 1; ++i)
	{
		_bodies[i].next = i + 1;
		_bodies[i].index = -1;
	}
	_bodies[_bdsize - 1].next = -1;
	_bodies[_bdsize - 1].index = -1;
}

Physicengine::~Physicengine()
{
	delete [] _bodies;
	delete [] _pairs;
}

void		Physicengine::new_body(Body **link, Shape *shape, Collider *collider)
{
	Body	*body;

	if (_bdfree == -1)
	{
		_bdfree = _bdsize;
		_bodies = resize(_bodies, _bdsize, _bdsize << 1);
		unsigned int i = 0;
		for (; i < _bdsize; ++i)
			*_bodies[i].link = _bodies + i;
		_bdsize <<= 1;
		for (; i < _bdsize - 1; ++i)
		{
			_bodies[i].next = i + 1;
			_bodies[i].index = -1;
		}
		_bodies[_bdsize - 1].next = -1;
		_bodies[_bdsize - 1].index = -1;
	}

	body = _bodies + _bdfree;
	_bdfree = _bodies[_bdfree].next;
	
	body->link = link;
	*body->link = body;
	body->shape = shape;
	body->collider = collider;
	
	body->velocity = 0.0f;
	body->acceleration = 0.0f;
	body->mass = 0.0f;
	body->elasticity = 0.0f;
	body->time = 0.0f;
}

void		Physicengine::init_body(Body *body)
{
	Aabb	aabb;

	body->shape->compute_aabb(aabb, body->position);
	if (body->dynamic)
		body->index = _dynamictree.add_aabb(aabb, (unsigned int)(body - _bodies));
	else
	{
		body->mass = INFINITY;
		body->index = _statictree.add_saabb(aabb, (unsigned int)(body - _bodies));
	}
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
		for (unsigned int i = 0; i < _prcount; ++i)
			if (_pairs[i].a == body->index || _pairs[i].b == body->index)
				_pairs[i].a = -1;
		if (body->dynamic)
			_dynamictree.remove_aabb(body->index);
		else
			_statictree.remove_aabb(body->index);
		body->index = -1;
	}
	else if (!body->dynamic)
		_statictree.remove_aabbs((unsigned int)(body - _bodies));
	body->next = _bdfree;
	_bdfree = (int)(body - _bodies);
}

void	Physicengine::add_aabb(Body *body, Aabb const &aabb)
{
	if (body->index == -1 && !body->dynamic)
		_statictree.add_saabb(aabb, (unsigned int)(body - _bodies));
}

void	Physicengine::remove_aabb(Body *body, Aabb const &aabb)
{
	if (body->index == -1 && !body->dynamic)
	{
		//_statictree.add_saabb(aabb, (unsigned int)(body - _bodies));
		//query aabb and select aabb.data == index of body
	}
}

/*
thread_local int	_currentquery;

void	_query(Body const *body, Physicengine *pe)
{
	if (body->index != -1 && body->dynamic)
	{
		_currentquery = body - pe->_bodies;
		pe->_dynamictree.query(body->aabb, pe, &Physicengine::_add_pair);
		pe->_statictree.query(body->aabb, pe, &Physicengine::_add_pair);
	}
}
*/
#define OUT_VEC(x) x[0] << " " << x[1] << " " << x[2]

void		Physicengine::tick(float const delta)
{
	_delta = delta;
	for (unsigned int i = 0; i < _bdsize; ++i)
	{
		if (_bodies[i].index != -1 && _bodies[i].dynamic)
		{
			_bodies[i].velocity += _bodies[i].acceleration * delta;
			_bodies[i].time = 0.0f;

			Aabb	aabb;

			_bodies[i].shape->compute_aabb(aabb, _bodies[i].position);
			_dynamictree.move_aabb(_bodies[i].index, aabb, _bodies[i].velocity * delta);
		}
	}

	_prcount = 0;
	_avoidquery = -1;
	_currenttime = 0.0f;
	//Lightthreadpool::get_instance().run_tasks<Caller_fa>(_bsize, _bodies, _query, this);
	for (unsigned int i = 0; i < _bdsize; ++i)
	{
		if (_bodies[i].index != -1 && _bodies[i].dynamic)
		{
			_currentquery = i;
			_dynamictree.query(_dynamictree._nodes[_bodies[i].index].aabb, this, &Physicengine::_add_pair);
			_statictree.query(_dynamictree._nodes[_bodies[i].index].aabb, this, &Physicengine::_add_pair);
		}
	}
	_sort_pairs(0);
	
	for (unsigned int i = 0; i < _prcount; ++i) // fix _currenttime
	{
		int const	a = _pairs[i].a;
		int const	b = _pairs[i].b;

		if (a == -1 || (_bodies[a].mass == INFINITY && _bodies[b].mass == INFINITY))
			continue;

		unsigned int const	currentcount = _prcount;
		_currenttime = _pairs[i].time - FLT_EPSILON;

		if (_bodies[a].mass != INFINITY)
			_bodies[a].position += _bodies[a].velocity * (_currenttime - _bodies[a].time);
		if (_bodies[b].mass != INFINITY)
			_bodies[b].position += _bodies[b].velocity * (_currenttime - _bodies[b].time);

		bool const	areaction = _bodies[a].collider->collide(_bodies[b].collider);
		bool const	breaction = _bodies[b].collider->collide(_bodies[a].collider);
		
		_solve(_bodies + a, _bodies + b, _pairs[i].normal);

		if (areaction && _bodies[a].mass != INFINITY)
		{
			_bodies[a].time = _currenttime;
			_avoidquery = b;
			_update_body(a, i);
		}
		if (breaction && _bodies[b].mass != INFINITY)
		{
			_bodies[b].time = _currenttime;
			_avoidquery = a;
			_update_body(b, i);
		}

		_sort_pairs(currentcount);
	}
	
	for (unsigned int i = 0; i < _bdsize; ++i)
		if (_bodies[i].index != -1 && _bodies[i].dynamic)
			_bodies[i].position += _bodies[i].velocity * (delta - _bodies[i].time);
}

void	Physicengine::_add_pair(int const aabbindex, int const bodyindex)
{
	if ((bodyindex > _currentquery || !_bodies[bodyindex].dynamic) && bodyindex != _avoidquery
		&& (_bodies[_currentquery].collider->should_collide(_bodies[bodyindex].collider) || _bodies[bodyindex].collider->should_collide(_bodies[_currentquery].collider)))
	{
		if (_bodies[bodyindex].index == -1)
			_bodies[bodyindex].position = _statictree._nodes[aabbindex].aabb.bottom;

		float			time;
		vec<float, 4>	normal;

		if (_bodies[_currentquery].shape->type < _bodies[bodyindex].shape->type)
			time = COLLISION_DISPATCHER[_bodies[_currentquery].shape->type][_bodies[bodyindex].shape->type](_bodies + _currentquery, _bodies + bodyindex, _currenttime, normal) + _currenttime;
		else
			time = COLLISION_DISPATCHER[_bodies[bodyindex].shape->type][_bodies[_currentquery].shape->type](_bodies + bodyindex, _bodies + _currentquery, _currenttime, normal) + _currenttime;

		if (time >= _currenttime && time <= _delta)
		{
			//spinlock.lock();
			if (_prcount >= _prsize)
			{
				_prsize <<= 1;
				_pairs = resize(_pairs, _prcount, _prsize);
			}
			_pairs[_prcount].a = _currentquery;
			_pairs[_prcount].b = bodyindex;
			_pairs[_prcount].aabb = aabbindex;
			_pairs[_prcount].time = time;
			_pairs[_prcount].normal = normal;
			++_prcount;
			//spinlock.unlock();
		}
	}
}

void	Physicengine::_update_body(int const index, int const start)
{
	for (unsigned int i = start; i < _prcount; ++i)
	{
		if (_pairs[i].a == index || _pairs[i].b == index)
			_pairs[i].a = -1;
	}

	Aabb	aabb;

	_bodies[index].shape->compute_aabb(aabb, _bodies[index].position);
	_dynamictree.move_aabb(_bodies[index].index, aabb, _bodies[index].velocity * _delta);

	_currentquery = index;
	_dynamictree.query(_dynamictree._nodes[_bodies[index].index].aabb, this, &Physicengine::_add_pair);
	_statictree.query(_dynamictree._nodes[_bodies[index].index].aabb, this, &Physicengine::_add_pair);
}

void	Physicengine::_sort_pairs(unsigned int const start)
{
	for (unsigned int i = start; i < _prcount; ++i)
	{
		Pair	a = _pairs[i];
		unsigned int j;
		for (j = i; j && _pairs[j - 1].time > a.time; --j)
			_pairs[j] = _pairs[j - 1];
		_pairs[j] = a;
	}
}

void		Physicengine::_solve(Body *x, Body *y, vec<float, 4> const &normal)
{
	float	mx = x->mass;
	float	my = y->mass;

	if (mx == INFINITY)
	{
		mx = 1.0;
		my = 0.0f;
	}
	else if (my == INFINITY)
	{
		mx = 1.0;
		my = 0.0f;
	}

	vec<float, 4> const	vx = x->velocity * normal;
	vec<float, 4> const	vy = y->velocity * -normal;
	vec<float, 4> const	mv = mx * vx + my * vy;
	float const	mm = mx + my;
	float const ee = x->elasticity * y->elasticity;

	if (x->mass != INFINITY)
		x->velocity += (ee * my * (vy - vx) + mv) / mm - vx;
	if (y->mass != INFINITY)
		y->velocity += (ee * mx * (vx - vy) + mv) / mm - vy;
}
