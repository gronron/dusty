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

#define GAP 0.5f
#define	MUL 2.0f

static void		merge_aabb(Aabb &x, Aabb const &y, Aabb const &z)
{
	x.bottom = min(y.bottom, z.bottom);
	x.top = max(y.top, z.top);
}

static float	perimeter(Aabb const &x)
{
	return (sum(x.top - x.bottom) * 2.0f);
}

static float	merged_perimeter(Aabb const &x, Aabb const &y)
{
	Aabb	a;
	
	a.bottom = min(x.bottom, y.bottom);
	a.top = max(x.top, y.top);
	return (sum(a.top - a.bottom) * 2.0f);
}

Aabbtree::Aabbtree() : _nsize(64), _nodes(0), _root(-1), _free(0)
{
	_nodes = new Node[_nsize];
	_nstack = new int[_nsize];
	
	for (unsigned int i = 0; i < _nsize - 1; ++i)
		_nodes[i].next = i + 1;
	_nodes[_nsize - 1].next = -1;
}

Aabbtree::~Aabbtree()
{
	delete [] _nodes;
	delete [] _nstack;
}

int		Aabbtree::add_aabb(Aabb const &aabb, int const data)
{
	int	index;
	
	index = _allocate_node();
	_nodes[index].aabb.bottom = aabb.bottom - GAP;
	_nodes[index].aabb.top = aabb.top + GAP;
	_nodes[index].data = data;
	_insert_leaf(index);
	return (index);
}

void			Aabbtree::remove_aabb(int const index)
{
	_remove_leaf(index);
	_free_node(index);
}

bool	Aabbtree::move_aabb(int const index, Aabb const &aabb, vec<float, 3> const &velocity) // need to correct
{
	if (_nodes[index].aabb.bottom <= aabb.top && _nodes[index].aabb.top >= aabb.bottom)
		return false;

	_remove_leaf(index);

	Aabb	a = aabb;
	a.bottom -= GAP;
	a.top += GAP;

	vec<float, 3>	b = velocity * MUL;
	
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (b[i] < 0.0f)
			a.bottom[i] += b[i];
		else
			a.top[i] += b[i];
	}

	_nodes[index].aabb = a;
	_insert_leaf(index);
	return (true);
}	

void	Aabbtree::query(Aabb const &aabb) const
{
	int	top = 0;
	
	_nstack[top++] = _root;
	while (top > 0)
	{
		int	index = _nstack[--top];
		if (index == -1)
			continue;

		if (_nodes[index].aabb.bottom <= aabb.top && _nodes[index].aabb.top >= aabb.bottom)
		{
			if (_nodes[index].right == -1)
			{
				bool proceed = callback->QueryCallback(nodeId);
				if (proceed == false)
					return;
			}
			else
			{
				_nstack[top++] = _nodes[index].left;
				_nstack[top++] = _nodes[index].right;
			}
		}
	}
}

int		Aabbtree::_allocate_node()
{
	int	index;

	if (_free == -1)
	{
		_free = _nsize;
		_nodes = resize(_nodes, _nsize, _nsize << 1);
		_nstack = resize(_nstack, _nsize, _nsize << 1);
		_nsize <<= 1;
		for (unsigned int i = _nsize >> 1; i < _nsize - 1; ++i)
			_nodes[i].next = i + 1;
		_nodes[_nsize - 1].next = -1;
	}
	
	index = _free;
	_free = _nodes[_free].next;
	_nodes[index].right = -1;
	return (index);
}

void	Aabbtree::_free_node(int const index)
{
	_nodes[index].next = _free;
	_free = index;
}

void	Aabbtree::_insert_leaf(int const index) // need to correct
{
	if (_root == -1)
	{
		_root = index;
		_nodes[index].parent = -1;
		_nodes[index].height = 0;
		return;
	}

	// Find the best sibling for this node
	Aabb leafaabb = _nodes[index].aabb;
	int i = _root;
	while (_nodes[i].right != -1)
	{
		int left = _nodes[i].left;
		int right = _nodes[i].right;

		float combinedArea = merged_perimeter(_nodes[i].aabb, leafaabb);

		// Cost of creating a new parent for this node and the new leaf
		float cost = 2.0f * combinedArea;
		// Minimum cost of pushing the leaf further down the tree
		float inheritanceCost = 2.0f * (combinedArea - perimeter(_nodes[i].aabb));

		float left_cost;
		float right_cost;
		
		if (_nodes[left].right == -1)
			left_cost = merged_perimeter(_nodes[left].aabb, leafaabb) + inheritanceCost;
		else
			left_cost = (merged_perimeter(_nodes[left].aabb, leafaabb) - perimeter(_nodes[left].aabb)) + inheritanceCost;

		if (_nodes[right].right == -1)
			right_cost = merged_perimeter(_nodes[right].aabb, leafaabb) + inheritanceCost;
		else
			right_cost = (merged_perimeter(_nodes[right].aabb, leafaabb) - perimeter(_nodes[right].aabb)) + inheritanceCost;

		if (cost < left_cost && cost < right_cost)
			break;
		i = left_cost <= right_cost ? left : right;
	}

	// Create a new parent.
	int oldparent = _nodes[i].parent;
	int newparent = _allocate_node();

	_nodes[newparent].parent = oldparent;
	merge_aabb(_nodes[newparent].aabb, leafaabb, _nodes[i].aabb);
	_nodes[newparent].height = _nodes[i].height + 1;
	_nodes[newparent].left = i;
	_nodes[newparent].right = index;
	_nodes[i].parent = newparent;
	_nodes[index].parent = newparent;

	if (oldparent != -1)
	{
		// The i was not the root.
		if (_nodes[oldparent].left == i)
			_nodes[oldparent].left = newparent;
		else
			_nodes[oldparent].right = newparent;
	}
	else
		_root = newparent;

	_balance(i);
}

void	Aabbtree::_remove_leaf(int const index)
{
	if (index == _root)
		_root = -1;
	else
	{
		int	parent = _nodes[index].parent;
		int	grandparent = _nodes[parent].parent;
		int	sibling = _nodes[parent].children[_nodes[parent].children[0] == index ? 1 : 0];

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
			_balance(index);
		}
	}
}

void	Aabbtree::_balance(int const index)
{
	for (int i = index; i != -1; i = _nodes[i].parent)
	{
		if (_nodes[i].right != -1 && _nodes[i].height >= 2)
		{
			int left = _nodes[i].left;
			int right = _nodes[i].right;
			int balance = _nodes[left].height - _nodes[right].height;
		
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
		
		int left = _nodes[i].left;
		int right = _nodes[i].right;

		merge_aabb(_nodes[i].aabb, _nodes[left].aabb, _nodes[right].aabb);
		_nodes[i].height = (_nodes[left].height > _nodes[right].height ? _nodes[left].height : _nodes[right].height) + 1;
	}
}

void	Aabbtree::_rotate(int const index, int const up, int const down) // a, b, c
{
	int left = _nodes[up].left; //d
	int right = _nodes[up].right; //e

	_nodes[up].left = index;
	_nodes[up].parent = _nodes[index].parent;
	_nodes[index].parent = up;

	if (_nodes[up].parent != -1)
	{
		if (_nodes[_nodes[up].parent].left == index)
			_nodes[_nodes[up].parent].left = up;
		else
			_nodes[_nodes[up].parent].right = up;
	}
	else
		_root = up;

	if (_nodes[left].height > _nodes[right].height)
	{
		_nodes[up].right = left;
		_nodes[index].left = right;
		_nodes[right].parent = index;
		
		merge_aabb(_nodes[index].aabb, _nodes[down].aabb, _nodes[right].aabb);
		merge_aabb(_nodes[up].aabb, _nodes[index].aabb, _nodes[left].aabb);

		_nodes[index].height = (_nodes[down].height > _nodes[right].height ? _nodes[down].height : _nodes[right].height) + 1;
		_nodes[up].height = (_nodes[index].height > _nodes[left].height ? _nodes[index].height : _nodes[left].height) + 1;
	}
	else
	{
		_nodes[up].right = right;
		_nodes[index].left = left;
		_nodes[left].parent = index;

		merge_aabb(_nodes[index].aabb, _nodes[down].aabb, _nodes[left].aabb);
		merge_aabb(_nodes[up].aabb, _nodes[index].aabb, _nodes[right].aabb);

		
		_nodes[index].height = (_nodes[down].height > _nodes[left].height ? _nodes[down].height : _nodes[left].height) + 1;
		_nodes[up].height = (_nodes[index].height > _nodes[right].height ? _nodes[index].height : _nodes[right].height) + 1;
	}
}
