/******************************************************************************
Copyright (c) 2015-207, Geoffrey TOURON
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

#include <cmath>
#include "math/vec.hpp"
#include "aabb.hpp"

struct		AabbNode
{
	Aabb	aabb;
	union
	{
		struct
		{
			int	parent;
			int	left;
			int	right;
			int	height;
		};
		struct
		{
			int	next;
			int	data;
		};
	};
};

union			OrderedAabbNode
{
	Aabb		aabb;
	struct
	{
		float	padding_[3];
		int		data;
		float	_padding[3];
		int		children;
	};
};

///////////////////////////////////////

class	AabbTree
{
public:

	unsigned int	_size;
	AabbNode		*_nodes;

	int				_root;
	int				_free;


	AabbTree();
	~AabbTree();

	void	reset();
	void	order_nodes();

	int		add_aabb(Aabb const &, int const data);
	int		add_saabb(Aabb const &, int const data);
	void	remove_aabb(int const);
	void	remove_aabbs(unsigned int const);
	bool	move_aabb(int const, Aabb const &, vec<float, 4> const &);
	bool	move_saabb(int const, Aabb const &);
	void	update_aabbdata(int const, int const data);
	void	update_aabbsdata(int const oldata, int const newdata);

	template<class T>
	void	query(Aabb const &, T *, void (T::*)(int const, int const)) const;
	template <class T>
	void	raycast_through(Ray const &ray, T* object, bool (T::*callback)(int const, int const, float const, float const)) const;
	template <class T>
	void	raycast(Ray const &ray, T* object, bool (T::*callback)(int const, int const, float const, float const)) const;

	int		_allocate_node();
	void	_free_node(int const);

	void	_insert_leaf(int const);
	void	_remove_leaf(int const);

	void	_balance(int const);
	void	_rotate(int const, int const, int const);
};

///////////////////////////////////////

class	OrderedAabbTree
{
	public:

		unsigned int	_size;
		unsigned int	_count;
		int				*_indexes;
		OrderedAabbNode	*_nodes;

		OrderedAabbTree();
		~OrderedAabbTree();

	void	construct_from(AabbTree const &);
	void	construct_from(unsigned int const size, OrderedAabbTree const * const);
};

///////////////////////////////////////

template<class T>
void	AabbTree::query(Aabb const &aabb, T* object, void (T::*callback)(int const, int const)) const
{
	if (_root != -1)
	{
		int	stack[32];
		int	top = 0;

		stack[top] = _root;
		do
		{
			int const	index = stack[top];

			if (aabb.is_overlapping(_nodes[index].aabb))
			{
				if (_nodes[index].right == -1)
					(object->*callback)(index, _nodes[index].data);
				else
				{
					stack[top++] = _nodes[index].left;
					stack[top++] = _nodes[index].right;
				}
			}
		}
		while (--top >= 0);
	}
}

template <class T>
void	AabbTree::raycast_through(Ray const &ray, T* object, bool (T::*callback)(int const, int const, float const, float const)) const
{
	if (_root != -1)
	{
		Ray	const	invray = { ray.origin, 1.0f / ray.direction };
		int			stack[32];
		int			top = 0;

		stack[top] = _root;
		do
		{
			int const	index = stack[top];
			float		near;
			float		far;

			if (intersect_invrayaabb(invray, _nodes[index].aabb, near, far))
			{
				if (_nodes[index].right == -1)
					(object->*callback)(index, _nodes[index].data, near, far);
				else
				{
					stack[top++] = _nodes[index].left;
					stack[top++] = _nodes[index].right;
				}
			}
		}
		while (--top >= 0);
	}
}

template <class T>
void	AabbTree::raycast(Ray const &ray, T* object, bool (T::*callback)(int const, int const, float const, float const)) const //need correction
{
	if (_root != -1)
	{
		Ray	const	invray = { ray.origin, 1.0f / ray.direction };
		int			stack[32];
		float		near_stack[32];
		int			top = 0;
		float		near = INFINITY;

		stack[top] = _root;
		near_stack[top] = 0.0f;
		do
		{
			if (near_stack[top] < near)
			{
				int const	index = stack[top];

				if (_nodes[index].right == -1)
				{
					if (!(object->*callback)(index, _nodes[index].data, near_stack[top], 0.0f))
						break;
				}
				else
				{
					float	lnear;
					float	lfar;
					float	rnear;
					float	rfar;

					int const	left = _nodes[index].left;
					int const	right = _nodes[index].right;
					bool const	ltrue = intersect_invrayaabb(invray, _nodes[left].aabb, lnear, lfar);
					bool const	rtrue = intersect_invrayaabb(invray, _nodes[right].aabb, rnear, rfar);

					if (lnear > rnear)
					{
						if (ltrue)
						{
							near_stack[top] = lnear;
							stack[top++] = left;
						}
						if (rtrue)
						{
							near_stack[top] = rnear;
							stack[top++] = right;
						}
					}
					else
					{
						if (rtrue)
						{
							near_stack[top] = rnear;
							stack[top++] = right;
						}
						if (ltrue)
						{
							near_stack[top] = lnear;
							stack[top++] = left;
						}
					}
				}
			}
		}
		while (--top >= 0);
	}
}
