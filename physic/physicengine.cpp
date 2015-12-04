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
}

void		Physicengine::init_body(Body *body)
{
	Aabb	aabb;

	body->shape->compute_aabb(aabb, body->position);
	if (body->dynamic)
		body->index = _dynamictree.add_aabb(aabb, (unsigned int)(body - _bodies));
	else
		body->index = _statictree.add_saabb(aabb, (unsigned int)(body - _bodies));
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
void		Physicengine::tick(float delta)
{
	for (unsigned int i = 0; i < _bdsize; ++i)
	{
		if (_bodies[i].index != -1 && _bodies[i].dynamic)
		{
			_bodies[i].prevposition = _bodies[i].position;
			_bodies[i].prevvelocity = _bodies[i].velocity;
			_bodies[i].position = _bodies[i].position + _bodies[i].velocity * delta + _bodies[i].acceleration * delta * delta * 0.5f; // + bd->ping
			_bodies[i].velocity = _bodies[i].velocity + _bodies[i].acceleration * delta;

			Aabb	aabb;

			_bodies[i].shape->compute_aabb(aabb, _bodies[i].prevposition);
			_dynamictree.move_aabb(_bodies[i].index, aabb, _bodies[i].velocity * delta);
		}
	}

	_prcount = 0;
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
	for (unsigned int i = 0; i < _prcount; ++i)
	{
		if (_bodies[_pairs[i].b].index == -1)
			_bodies[_pairs[i].b].prevposition = _statictree._nodes[_pairs[i].aabb].aabb.bottom;
	
		float const time = aabox_aabox(_bodies + _pairs[i].a, _bodies + _pairs[i].b);
		std::cout << "::" << time << std::endl;

		if (time >= 0.0f && time <= delta)
		{
			_bodies[_pairs[i].a].collider->collide(_bodies[_pairs[i].b].collider);
			_bodies[_pairs[i].b].collider->collide(_bodies[_pairs[i].a].collider);
		}
	}
}

void	Physicengine::_add_pair(int const aabbindex, int const bodyindex)
{
	if (bodyindex > _currentquery || !_bodies[bodyindex].dynamic)
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
		++_prcount;
		//spinlock.unlock();
	}
}
