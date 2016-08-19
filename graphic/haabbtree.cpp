/******************************************************************************
Copyright (c) 2015-2016, Geoffrey TOURON
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

#include "haabbtree.hpp"

Haabbtree::Haabbtree() : Aabbtree(), _realroot(-1), _transitory(false)
{

}

void	Haabbtree::add_transient_aabb(Aabb const &aabb, int const data)
{
	if (!_transitory)
	{
		_transitory = true;
		_realroot = _root;
		_root = -1;
	}

	int const	index = _allocate_node();

	_nodes[index].left = -1;
	_nodes[index].right = -1;
	_nodes[index].height = 0;
	_nodes[index].aabb.bottom = aabb.bottom;
	_nodes[index].aabb.top = aabb.top;
	_nodes[index].data = data;
	_insert_leaf(index);
}

void	Haabbtree::attach_transient_tree()
{
	if (_transitory && _realroot != -1)
	{
		int const	newroot = _allocate_node();
		
		_nodes[newroot].parent = -1;
		_nodes[newroot].left = _realroot;
		_nodes[newroot].right = _root;
		_nodes[newroot].height = (_nodes[_realroot].height > _nodes[_root].height ? _nodes[_realroot].height : _nodes[_root].height) + 1;
		_nodes[newroot].aabb.merge(_nodes[_realroot].aabb, _nodes[_root].aabb);
		_root = newroot;
	}
}

void	Haabbtree::delete_transient_tree()
{
	if (_transitory)
	{
		_transitory = false;

		int	stack[32];
		int	top = 0;

		if (_realroot == -1)
			stack[top] = _root;
		else
		{
			stack[top] = _nodes[_root].right;

			_free_node(_root);
			_root = _realroot;
			_realroot = -1;
		}

		do
		{
			int const	index = stack[top];

			if (_nodes[index].right != -1)
			{
				stack[top++] = _nodes[index].left;
				stack[top++] = _nodes[index].right;
			}
			_free_node(index);
		}
		while (--top >= 0);
	}
}

int				Maabbtree::close_current_tree()
{
	int const	index = _root;
	_root = -1;
	return (index);
}

void		Maabbtree::delete_tree(int const root)
{
	int	stack[32];
	int	top = 0;
	
	stack[top++] = root;

	do
	{
		int const	index = stack[--top];

		if (_nodes[index].right != -1)
		{
			stack[top++] = _nodes[index].left;
			stack[top++] = _nodes[index].right;
		}
		_free_node(index);
	}
	while (top);
}
