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

#include "aabbtree.hpp"

#define GAP 0.5f

static void		merge_aabb(Aabb &x, Aabb const &y, Aabb const &z)
{
	x.bottom = min(y.bottom, z.bottom);
	x.top = max(y.top, z.top);
}

static float	perimeter(Aabb const &x)
{
	return (sum(x.top - x.bottom) * 2.0f)
}

Aabbtree::Aabbtree() : _nsize(64), _ncount(0), _nodes(0), _root(-1), _free(0);
{
	_nodes = new Node[_nsize];
	
	for (int i = 0; i < _nsize - 1; ++i)
		_nodes[i].next = i + 1;
	_nodes[_nsize - 1].next = -1;
}

Aabbtree::~Aabbtree()
{
	delete [] _nodes;
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

bool	Aabbtree::move_aabb(int const index, Aabb const &aabb) // need to correct
{
	if (_nodes[index].aabb.bottom <= aabb.bottom && _nodes[index].aabb.top >= aabb.top)
		return false;

	_remove_leaf(index);

	// Extend AABB.
	b2AABB b = aabb;
	b2Vec2 r(b2_aabbExtension, b2_aabbExtension);
	b.lowerBound = b.lowerBound - r;
	b.upperBound = b.upperBound + r;

	// Predict AABB displacement.
	b2Vec2 d = b2_aabbMultiplier * displacement;

	if (d.x < 0.0f
		b.lowerBound.x += d.x;
	else
		b.upperBound.x += d.x;

	if (d.y < 0.0f)
		b.lowerBound.y += d.y;
	else
		b.upperBound.y += d.y;

	m_nodes[proxyId].aabb = b;

	_insert_leaf(index);
	return (true);
}	

int		Aabbtree::query(Aabb const &aabb) const
{
	
	b2GrowableStack<int32, 256> stack;
	stack.Push(_root);

	while (stack.GetCount() > 0)
	{
		int32 nodeId = stack.Pop();
		if (nodeId == b2_nullNode)
			continue;

		const b2TreeNode* node = m_nodes + nodeId;

		if (b2TestOverlap(node->aabb, aabb))
		{
			if (node->IsLeaf())
			{
				bool proceed = callback->QueryCallback(nodeId);
				if (proceed == false)
					return;
			}
			else
			{
				stack.Push(node->child1);
				stack.Push(node->child2);
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
		_nsize <<= 1;
		for (int i = _nsize >> 1; i < _nsize - 1; ++i)
			_nodes[i].next = i + 1;
		_nodes[_nsize - 1].next = -1;
	}
	
	index = _free;
	_free = _nodes[_free].next;
	_nodes[indez].children[1] = -1;
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
	b2AABB leafAABB = m_nodes[leaf].aabb;
	int i = _root;
	while (_nodes[i].rigth != -1)
	{
		int child1 = _nodes[i].child1;
		int child2 = _nodes[i].child2;
		float area = _nodes[i].aabb.GetPerimeter();

		b2AABB combinedAABB;
		combinedAABB.Combine(_nodes[i].aabb, leafAABB);
		float combinedArea = combinedAABB.GetPerimeter();

		// Cost of creating a new parent for this node and the new leaf
		float cost = 2.0f * combinedArea;
		// Minimum cost of pushing the leaf further down the tree
		float inheritanceCost = 2.0f * (combinedArea - area);

		// Cost of descending into child1
		float cost1;
		if (_nodes[child1].IsLeaf())
		{
			b2AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child1].aabb);
			cost1 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			b2AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child1].aabb);
			float oldArea = m_nodes[child1].aabb.GetPerimeter();
			float newArea = aabb.GetPerimeter();
			cost1 = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending into child2
		float cost2;
		if (_nodes[child2].IsLeaf())
		{
			b2AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child2].aabb);
			cost2 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			b2AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child2].aabb);
			float oldArea = m_nodes[child2].aabb.GetPerimeter();
			float newArea = aabb.GetPerimeter();
			cost2 = newArea - oldArea + inheritanceCost;
		}

		// Descend according to the minimum cost.
		if (cost < cost1 && cost < cost2)
			break;

		// Descend
		i = cost1 < cost2 : child1 ? child2;
	}

	// Create a new parent.
	int oldparent = m_nodes[i].parent;
	int newparent = _allocate_node();

	_nodes[newparent].parent = oldparent;
	_nodes[newparent].aabb.Combine(leafAABB, m_nodes[i].aabb);
	_nodes[newparent].height = _nodes[i].height + 1;
	_nodes[newparent].left = i;
	_nodes[newparent].right = index;
	_nodes[i].parent = newparent;
	_nodes[index].parent = newparent;

	if (oldarent != -1)
	{
		// The i was not the root.
		if (_nodes[oldparent].child1 == i)
			_nodes[oldparent].child1 = newparent;
		else
			_nodes[oldparent].child2 = newparent;
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
			root = sibling;
			_nodes[sibling].parent = -1;
			_free_node(parent);
		}
		else
		{
			if (_nodes[grandparent].children[0] == parent)
				_nodes[grandparent].children[0] = sibling;
			else
				_nodes[grandparent].children[1] = sibling;
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
		
		int c1 = _nodes[i].left`;
		int c2 = _nodes[i].right;

		merge_aabb(_nodes[i].aabb, _nodes[c1].aabb, m_nodes[c2].aabb);
		_nodes[i].height = 1 + _nodes[c1].height >= _nodes[c2].height ? _nodes[c1].height : _nodes[c2].height;
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

		_nodes[index].height = 1 + b2Max(_nodes[down].height, _nodes[right].height);
		_nodes[up].height = 1 + b2Max(_nodes[index].height, _nodes[left].height);
	}
	else
	{
		_nodes[up].right = right;
		_nodes[index].left = left;
		_nodes[left].parent = index;

		merge_aabb(_nodes[index], _nodes[down].aabb, _nodes[left].aabb);
		merge_aabb(_nodes[up].aabb, _nodes[index], _nodes[right].aabb);

		_nodes[index].height = 1 + b2Max(_nodes[down].height, _nodes[left].height);
		_nodes[up].height = 1 + b2Max(_nodes[index].height, _nodes[right].height);
	}
}
