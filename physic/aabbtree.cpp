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
// this code is greatly inspired by Erin Catto's b2DynamicTree form Box2d

#include "new.hpp"
#include "math/vec_util.hpp"
#include "aabbtree.hpp"

#define GAP 0.2f
#define	MUL 2.0f

inline float	perimeter(Aabb const &x)
{
	return (sum(x.top - x.bottom) * 2.0f);
}

inline float	merged_perimeter(Aabb const &x, Aabb const &y)
{
	Aabb		a;
	
	a.bottom = min(x.bottom, y.bottom);
	a.top = max(x.top, y.top);
	return (sum(a.top - a.bottom) * 2.0f);
}

Aabbtree::Aabbtree() : _size(1024), _nodes(0), _root(-1), _free(0)
{
	_nodes = new Node[_size];
	
	for (unsigned int i = 0; i < _size - 1; ++i)
		_nodes[i].next = i + 1;
	_nodes[_size - 1].next = -1;
}

Aabbtree::~Aabbtree()
{
	delete [] _nodes;
}

int				Aabbtree::add_aabb(Aabb const &aabb, int const data)
{
	int const	index = _allocate_node();
	
	_nodes[index].left = -1;
	_nodes[index].right = -1;
	_nodes[index].height = 0;
	_nodes[index].aabb.bottom = aabb.bottom - GAP;
	_nodes[index].aabb.top = aabb.top + GAP;
	_nodes[index].data = data;
	_insert_leaf(index);
	return (index);
}

int				Aabbtree::add_saabb(Aabb const &aabb, int const data)
{
	int const	index = _allocate_node();

	_nodes[index].left = -1;
	_nodes[index].right = -1;
	_nodes[index].height = 0;
	_nodes[index].aabb.bottom = aabb.bottom;
	_nodes[index].aabb.top = aabb.top;
	_nodes[index].data = data;
	_insert_leaf(index);
	return (index);
}

void			Aabbtree::remove_aabb(int const index)
{
	_remove_leaf(index);
	_free_node(index);
}

bool	Aabbtree::move_aabb(int const index, Aabb const &aabb, vec<float, 4> const &velocity) // need to correct
{
	if (_nodes[index].aabb.is_containing(aabb))
		return (false);

	Aabb	a = aabb;
	a.bottom -= GAP;
	a.top += GAP;

	vec<float, 4> const	b = velocity * MUL;
	
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (b[i] < 0.0f)
			a.bottom[i] += b[i];
		else
			a.top[i] += b[i];
	}
	_nodes[index].aabb = a;

	_remove_leaf(index);
	_insert_leaf(index);
	return (true);
}

bool	Aabbtree::move_saabb(int const index, Aabb const &aabb)
{
	if (_nodes[index].aabb.is_containing(aabb))
		return (false);

	_nodes[index].aabb = aabb;

	_remove_leaf(index);
	_insert_leaf(index);
	return (true);
}

int		Aabbtree::_allocate_node()
{
	int	index;

	if (_free == -1)
	{
		_free = _size;
		_nodes = resize(_nodes, _size, _size << 1);
		_size <<= 1;
		for (unsigned int i = _size >> 1; i < _size - 1; ++i)
			_nodes[i].next = i + 1;
		_nodes[_size - 1].next = -1;
	}

	index = _free;
	_free = _nodes[_free].next;
	return (index);
}

void	Aabbtree::_free_node(int const index)
{
	_nodes[index].next = _free;
	_free = index;
}

void	Aabbtree::_insert_leaf(int const index)
{
	if (_root == -1)
	{
		_root = index;
		_nodes[index].parent = -1;
		return;
	}

	Aabb leafaabb = _nodes[index].aabb;
	int i = _root;
	while (_nodes[i].right != -1)
	{
		int const	left = _nodes[i].left;
		int const	right = _nodes[i].right;
		
		float const	cost = merged_perimeter(_nodes[i].aabb, leafaabb) * 2.0f;
		float const	inheritance_cost = cost - perimeter(_nodes[i].aabb) * 2.0f;

		float left_cost = merged_perimeter(_nodes[left].aabb, leafaabb) + inheritance_cost;
		float right_cost = merged_perimeter(_nodes[right].aabb, leafaabb) + inheritance_cost;

		if (_nodes[left].right != -1)
			left_cost -= perimeter(_nodes[left].aabb);
		if (_nodes[right].right != -1)
			right_cost -= perimeter(_nodes[right].aabb);

		if (cost < left_cost && cost < right_cost)
			break;
		i = left_cost < right_cost ? left : right;
	}
	
	int const	oldparent = _nodes[i].parent;
	int const	newparent = _allocate_node();

	_nodes[newparent].parent = oldparent;
	_nodes[newparent].aabb.merge(leafaabb, _nodes[i].aabb);
	_nodes[newparent].height = _nodes[i].height + 1;
	_nodes[newparent].left = i;
	_nodes[newparent].right = index;
	_nodes[i].parent = newparent;
	_nodes[index].parent = newparent;

	if (oldparent != -1)
	{
		if (_nodes[oldparent].left == i)
			_nodes[oldparent].left = newparent;
		else
			_nodes[oldparent].right = newparent;
	}
	else
		_root = newparent;
	_balance(_nodes[index].parent);
}

void	Aabbtree::_remove_leaf(int const index)
{
	if (index == _root)
		_root = -1;
	else
	{
		int const	parent = _nodes[index].parent;
		int const	grandparent = _nodes[parent].parent;
		int const	sibling = _nodes[parent].left != index ? _nodes[parent].left : _nodes[parent].right;

		if (grandparent == -1)
		{
			_root = sibling;
			_nodes[sibling].parent = -1;
			_free_node(parent);
		}
		else
		{
			if (_nodes[grandparent].left == parent)
				_nodes[grandparent].left = sibling;
			else
				_nodes[grandparent].right = sibling;
			_nodes[sibling].parent = grandparent;
			_free_node(parent);
			_balance(grandparent);
		}
	}
}

void	Aabbtree::_balance(int const index)
{
	for (int i = index; i != -1; i = _nodes[i].parent)
	{
		if (_nodes[i].right != -1 && _nodes[i].height >= 2)
		{
			int const	left = _nodes[i].left;
			int const	right = _nodes[i].right;
			int const	balance = _nodes[left].height - _nodes[right].height;
		
			if (balance > 1)
			{
				_rotate(i, left, right);
				i = left;
			}	
			else if (balance < -1)
			{
				_rotate(i, right, left);
				i = right;
			}
		}
		
		int const	left = _nodes[i].left;
		int const	right = _nodes[i].right;

		_nodes[i].height = (_nodes[left].height > _nodes[right].height ? _nodes[left].height : _nodes[right].height) + 1;
		_nodes[i].aabb.merge(_nodes[left].aabb, _nodes[right].aabb);
	}
}

void			Aabbtree::_rotate(int const up, int const down, int const sibling)
{
	int const	left = _nodes[down].left;
	int const	right = _nodes[down].right;

	_nodes[down].left = up;
	_nodes[down].parent = _nodes[up].parent;
	_nodes[up].parent = down;

	if (_nodes[down].parent != -1)
	{
		if (_nodes[_nodes[down].parent].left == up)
			_nodes[_nodes[down].parent].left = down;
		else
			_nodes[_nodes[down].parent].right = down;
	}
	else
		_root = down;

	if (_nodes[left].height > _nodes[right].height)
	{
		_nodes[down].right = left;
		if (_nodes[up].left == down)
			_nodes[up].left = right;
		else
			_nodes[up].right = right;
		_nodes[right].parent = up;
		
		_nodes[up].height = (_nodes[sibling].height > _nodes[right].height ? _nodes[sibling].height : _nodes[right].height) + 1;
		_nodes[down].height = (_nodes[up].height > _nodes[left].height ? _nodes[up].height : _nodes[left].height) + 1;
		
		_nodes[up].aabb.merge(_nodes[sibling].aabb, _nodes[right].aabb);
		_nodes[down].aabb.merge(_nodes[up].aabb, _nodes[left].aabb);
	}
	else
	{
		_nodes[down].right = right;
		if (_nodes[up].left == down)
			_nodes[up].left = left;
		else
			_nodes[up].right = left;
		_nodes[left].parent = up;

		_nodes[up].height = (_nodes[sibling].height > _nodes[left].height ? _nodes[sibling].height : _nodes[left].height) + 1;
		_nodes[down].height = (_nodes[up].height > _nodes[right].height ? _nodes[up].height : _nodes[right].height) + 1;

		_nodes[up].aabb.merge(_nodes[sibling].aabb, _nodes[left].aabb);
		_nodes[down].aabb.merge(_nodes[up].aabb, _nodes[right].aabb);
	}
}
