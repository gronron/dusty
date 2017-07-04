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

#pragma once

#include <shared_mutex>
#include "body.hpp"
#include "aabbtree.hpp"

class	Shape;
class	Collider;

class	Querycallback
{
	public:

		virtual ~Querycallback() { }

		virtual void	report_encounter(Body *, int const) = 0;
};

class	Raycastcallback
{
	public:

		virtual ~Raycastcallback() { }

		virtual bool	report_encounter(Body *, int const, float const, float const) = 0;
};

class	Physicengine
{
	public:

		struct				Pair
		{
			int				a;
			int				b;
			int				aabb;
			float			time;
			vec<float, 4>	normal;
		};


		unsigned int	_bdcount;
		unsigned int	_bdsize;
		int				_bdfree;
		Body			*_bodies;

		unsigned int	_prcount;
		unsigned int	_prsize;
		Pair			*_pairs;

		Aabbtree		_dynamictree;
		Aabbtree		_statictree;

		std::shared_mutex	_mutex;

		int				_currentquery;
		int				_avoidquery;
		float			_currenttime;
		float			_delta;


		Physicengine();
		~Physicengine();

		void	new_body(Body **, Shape *, Collider *);
		void	init_body(Body *);
		void	delete_body(Body *);
		
		int		add_aabb(Body *, Aabb const &);
		void	remove_aabb(Body *, int const);
		void	remove_aabbs(Body *);
		void	remove_aabbs(Body *, Aabb const &);
		
		void	query(Aabb const &, Querycallback *, bool const, bool const) const;
		void	query(vec<float, 4> const &, float const, Querycallback *, bool const, bool const) const;
		void	raycast_through(Ray const &, Raycastcallback *, bool const, bool const) const;
		void	raycast(Ray const &, Raycastcallback *, bool const, bool const) const;

		void	tick(float const);

		void	_add_pair(int const, int const);
		void	_sort_pairs();
		void	_sort_pairs(unsigned int const);
		void	_update_body(int const, int const);
		void	_solve(Body *, Body *, vec<float, 4> const &);
};
