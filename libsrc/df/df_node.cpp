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

#include <iostream>
#include <cstdlib>
#include "df_node.hpp"

DFRoot::DFRoot()
	: _roots(8)
	, _nodes(64)
	, _data_size(0)
	, _data_storage(nullptr)
{

}

DFRoot::~DFRoot()
{
	delete [] _data_storage;
}

DFNode const * const		DFRoot::get(std::string const & researched_name) const
{
	DFNode const * const	node = nullptr;

	for (unsigned int i = 0; i < _roots.number; ++i)
		if ((node = _roots[i]->get(researched_name)) != nullptr)
			return (node);
}

DFNode const * const		DFRoot::get(std::string const & researched_name, Type const expected_type, unsigned int const expected_size, void * const out) const;
{
	DFNode const * const	node = nullptr;

	for (unsigned int i = 0; i < _roots.number; ++i)
		if ((node = _roots[i]->get(researched_name, expected_size, expected_size, out)) != nullptr)
			return (node);
}

DFNode const * const		DFRoot::safe_get(std::string const & researched_name, Type const expected_type, unsigned int const expected_size) const;
{
	DFNode const * const	node = nullptr;

	for (unsigned int i = 0; i < _roots.number; ++i)
		if ((node = _roots[i]->get(researched_name, expected_type, expected_size)) != nullptr)
			return (node);
}

///////////////////////////////////////////////////////////////////////////////

DFNode::DFNode() : size(0), type(Type::NONE)
{
	//user_type[0] = '\0';
	//name[0] = '\0';
}

DFNode::~DFNode()
{
	if (type == BLOCK)
	{
		for (unsigned int i = 0; i < size; ++i)
			delete node[i];
		delete [] node;
	}
	else if (type == STRING)
	{
		for (unsigned int i = 0; i < size; ++i)
			delete [] cstr[i];
	}
	if (data_storage)
		free(data_storage);
}

DFNode const	*DFNode::get(std::string const &researched_nane) const
{
	size_t		a;
	std::string	prefix;
	std::string	suffix;

	if (!researched_nane.size() || researched_nane == ".")
		return (this);
	else if (type == BLOCK)
	{
		a = researched_nane.find('.');
		prefix = researched_nane.substr(0, a);
		if (a != std::string::npos)
			suffix = researched_nane.substr(a + 1);
		for (unsigned int i = 0; i < size; ++i)
		{
			if (node[i]->name == prefix)
				return (node[i]->get(suffix));
		}
	}
	return (0);
}

DFNode const		*DFNode::get(std::string const &researched_name, Type expected_type, unsigned int expected_size, void *out) const
{
	DFNode const	*rnode;

	if (!(rnode = get(researched_name)) || rnode->type != expected_type || rnode->size < expected_size)
		return (0);
	else
	{
		if (rnode->type > BLOCK && out)
			memcpy(rnode->data, out, expected_size);
		return (rnode);
	}
}

DFNode const		*DFNode::safe_get(std::string const &researched_name, Type expected_type, unsigned int expected_size) const
{
	DFNode const	*rnode;

	if (!(rnode = get(researched_name)))
	{
		std::cerr << "error! DFNode::safe_get() fails to find: " << researched_name << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (rnode->type != expected_type)
	{
		std::cerr << "error! DFNode::safe_get() type doesn't match for " << researched_name << ": expected type " << expected_type << " actual type " << rnode->type << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (rnode->size < expected_size)
	{
		std::cerr << "error! DFNode::safe_get() size is too low for " << researched_name << ": expected size " << expected_size << " actual size " << rnode->size << std::endl;
		exit(EXIT_FAILURE);
	}
	return (rnode);
}

void	DFNode::print() const
{
	std::cout << name << ' ' << type << std::endl;
	if (type == BLOCK)
	{
		std::cout << '{' << std::endl;
		for (unsigned int i = 0; i < size; ++i)
			node[i]->print();
		std::cout << '}' << std::endl;
	}
	else if (type == INT)
	{
		for (unsigned int i = 0; i < size; ++i)
			std::cout << nbr[i] << ' ';
	}
	else if (type == FLOAT)
	{
		for (unsigned int i = 0; i < size; ++i)
			std::cout << flt[i] << ' ';
	}
	else if (type == DOUBLE)
	{
		for (unsigned int i = 0; i < size; ++i)
			std::cout << dbl[i] << ' ';
	}
	else if (type == STRING)
	{
		for (unsigned int i = 0; i < size; ++i)
			std::cout << cstr[i] << ' ';
	}
	std::cout << std::endl;
}
