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
// this code is greatly inspired by Erin Catto's b2DynamicTree form Box2d

#include "new.hpp"
#include "math/vec_util.hpp"
#include "AabbTree.hpp"

#define GAP 0.2f
#define	MUL 2.0f

inline float			surfacearea(Aabb const &x)
{
	vec<float, 4> const	a = x.top - x.bottom;

	return (a[0] * a[1] + a[0] * a[2] + a[1] * a[2]);
}

inline float			merged_surfacearea(Aabb const &x, Aabb const &y)
{
	vec<float, 4> const	a = vmax(x.top, y.top) - vmin(x.bottom, y.bottom);

	return (a[0] * a[1] + a[0] * a[2] + a[1] * a[2]);
}

AabbTree::AabbTree() : _size(1024), _nodes(nullptr), _root(-1), _free(0)
{
	_nodes = new AabbNode[_size];

	for (unsigned int i = 0; i < _size - 1; ++i)
	{
		_nodes[i].next = i + 1;
		_nodes[i].height = -1;
	}
	_nodes[_size - 1].next = -1;
	_nodes[_size - 1].height = -1;
}

AabbTree::~AabbTree()
{
	delete [] _nodes;
}

void	AabbTree::reset()
{
	_root = -1;
	_free = 0;
	for (unsigned int i = 0; i < _size - 1; ++i)
	{
		_nodes[i].next = i + 1;
		_nodes[i].height = -1;
	}
	_nodes[_size - 1].next = -1;
	_nodes[_size - 1].height = -1;
}

int				AabbTree::add_aabb(Aabb const &aabb, int const data)
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

int				AabbTree::add_saabb(Aabb const &aabb, int const data)
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

void	AabbTree::remove_aabb(int const index)
{
	if (!_nodes[index].height)
	{
		_remove_leaf(index);
		_free_node(index);
	}
}

void	AabbTree::remove_aabbs(unsigned int const data)
{
	for (unsigned int i = 0; i < _size; ++i)
	{
		if (_nodes[i].height == 0 && _nodes[i].data == data)
		{
			_remove_leaf(i);
			_free_node(i);
		}
	}
}

bool					AabbTree::move_aabb(int const index, Aabb const &aabb, vec<float, 4> const &velocity)
{
	Aabb				a = aabb;
	vec<float, 4> const	b = velocity * MUL;

	for (unsigned int i = 0; i < 3; ++i)
	{
		if (b[i] < 0.0f)
			a.bottom[i] += b[i];
		else
			a.top[i] += b[i];
	}

	if (_nodes[index].aabb.is_containing(a))
		return (false);

	a.bottom -= GAP;
	a.top += GAP;
	_nodes[index].aabb = a;

	_remove_leaf(index);
	_insert_leaf(index);
	return (true);
}

bool	AabbTree::move_saabb(int const index, Aabb const &aabb)
{
	if (_nodes[index].aabb.is_containing(aabb))
		return (false);

	_nodes[index].aabb = aabb;

	_remove_leaf(index);
	_insert_leaf(index);
	return (true);
}

void	AabbTree::update_aabbdata(int const index, int const data)
{
	_nodes[index].data = data;
}

void	AabbTree::update_aabbsdata(int const olddata, int const newdata)
{
	for (unsigned int i = 0; i < _size; ++i)
		if (_nodes[i].height == 0 && _nodes[i].data == olddata)
			 _nodes[i].data = newdata;
}

int		AabbTree::_allocate_node()
{
	int	index;

	if (_free == -1)
	{
		_free = _size;
		_nodes = resize(_nodes, _size, _size << 1);
		_size <<= 1;
		for (unsigned int i = _size >> 1; i < _size - 1; ++i)
		{
			_nodes[i].next = i + 1;
			_nodes[i].height = -1;
		}
		_nodes[_size - 1].next = -1;
		_nodes[_size - 1].height = -1;
	}

	index = _free;
	_free = _nodes[_free].next;
	return (index);
}

void	AabbTree::_free_node(int const index)
{
	_nodes[index].height = -1;
	_nodes[index].next = _free;
	_free = index;
}

void	AabbTree::_insert_leaf(int const index)
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

		float left_cost = merged_surfacearea(_nodes[left].aabb, leafaabb);
		float right_cost = merged_surfacearea(_nodes[right].aabb, leafaabb);

		if (_nodes[left].right != -1)
			left_cost -= surfacearea(_nodes[left].aabb);
		if (_nodes[right].right != -1)
			right_cost -= surfacearea(_nodes[right].aabb);

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

void	AabbTree::_remove_leaf(int const index)
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

void	AabbTree::_balance(int const index)
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

void			AabbTree::_rotate(int const up, int const down, int const sibling)
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

///////////////////////////////////////

OrderedAabbTree::OrderedAabbTree() : _size(0), _count(0), _indexes(nullptr), _nodes(nullptr)
{

}

OrderedAabbTree::~OrderedAabbTree()
{
	if (_size)
	{
		delete [] _indexes;
		delete [] _nodes;
	}
}

void	OrderedAabbTree::construct_from(const AabbTree &aabbtree)
{
	_count = 0;

	if (aabbtree._root == -1)
		return;

	if (_size != aabbtree._size)
	{
		_size = aabbtree._size;
		if (_nodes)
		{
			delete [] _indexes;
			delete [] _nodes;
		}
		_indexes = new int[_size];
		_nodes = new OrderedAabbNode[_size];
	}

	_indexes[_count++] = aabbtree._root;

	for (unsigned int i = 0; i < _count; ++i)
	{
		unsigned int const	index = _indexes[i];

		_nodes[i].aabb = aabbtree._nodes[index].aabb;

		if (aabbtree._nodes[index].right != -1)
		{
			_nodes[i].data = -1;
			_nodes[i].children = _count;
			_indexes[_count++] = aabbtree._nodes[index].left;
			_indexes[_count++] = aabbtree._nodes[index].right;
		}
		else
		{
			_nodes[i].data = aabbtree._nodes[index].data;
			_nodes[i].children = -1;
		}
	}
}

#ifdef SECONDI

struct	Split
{
	float score;
	int	  index;
};

Split	search_split(OrderedAabbNode const * const aabbs, Center const * const centers, unsigned int begin, unsigned int end, float * const score)
{
	Split	best_split = { FLT_MAX, 0 };
	Aabb	aabb;

	aabb = aabbs[centers[begin].index].aabb;
	for (unsigned int i = begin + 1; i < end; ++i)
	{
		aabb.merge(aabbs[centers[i].index].aabb);

		score[i] = surfacearea(aabb);
	}

	aabb = aabbs[centers[end - 1].index].aabb;
	for (unsigned int i = end - 1; i >= begin; --i)
	{
		aabb.merge(aabbs[centers[i].index].aabb);
		score[i] += surfacearea(aabb);

		if (score[i] < best_split.score)
		{
			best_split.score = score[i];
			best_split.index = i;
		}
	}

	return (best_split);
}

void	construct_from(unsigned int const size, OrderedAabbNode const * const aabbs)
{
	struct				Center
	{
		float			center;
		unsigned int	index;
	}

	struct				Range
	{
		unsigned int	begin;
		unsigned int	end;
	};

	Center * const		centers[3];
	bool * const		flags = new bool[size];
	float * const		scores_buffer = new float[size];

	Range				stack[32];
	unsigned int		top = 1;

	stack[0] = { 0, size };

	centers[0] = new Center[size * 3];
	centers[1] = centers[0] + size;
	centers[2] = centers[1] + size;

	for (unsigned int i = 0; i < size; ++i)
	{
		vec<float, 4> const center = (aabbs[i].bottom + aabbs[i].top) * 0.5f;

		centers[0][i].center = center[0];
		indices[0][i].index = i;

		centers[1][i].center = center[1];
		indices[1][i].index = i;

		centers[2][i].center = center[2];
		indices[2][i].index = i;
	}

	//threadpooling
	auto cmp = [](auto const &x, auto const &y) { return x.center < y.center; };
	std::sort(indexs[0], indexs[0] + size, cmp);
	std::sort(indexs[1], indexs[1] + size, cmp);
	std::sort(indexs[2], indexs[2] + size, cmp);

	while (top)
	{
		Split split[3];

		splits[0] = search_split(aabbs, centers[0], Range[].begin, Range[].end, scores_buffer);

		unsigned int const splitaxis = 0;
		unsigned int const second = (splitaxis + 1) % 3;
		unsigned int const third = (splitaxis + 1) % 3;

		for (unsigned int i = Range[].begin; i < Range[].end; ++i)
		{
			flags[centers[axis][i]] = i <= split[axis].index;
		}

		for (unsigned int i = Range[].begin; i < Range[].end; ++i)
		{

		}

		//recopy on other axis
		//split
		//rearange
	}

	delete [] scores_buffer;
	delete [] flags;
	delete [] centers[0];
}

/*
		vec<float, 3> axis_score = {0.0f, 0.0f, 0.0f};
		vec<float, 3> axis_size;

		axis_size[0] = indexs[0][0].size;
		axis_size[1] = indexs[1][0].size;
		axis_size[2] = indexs[2][0].size;

		for (unsigned int i = 1; i < size; ++i)
		{
			axis_score += indexs[0][i - 1].center - index[0][i].center;

			axis_size[0] += indexs[0][i].size;
			axis_size[1] += indexs[1][i].size;
			axis_size[2] += indexs[2][i].size;
		}

		axis_score /= axis_size;

		int best_axis;

		if (axis_score[0] > axis_score[1])
		{
			if (axis_score[0] > axis_score[2])
				best_axis = 0;
			else
				best_axis = 2;
		}
		else
		{
			if (axis_score[1] > axis_score[2])
				best_axis = 1;
			else
				best_axis = 2;
		}
*/

#endif
