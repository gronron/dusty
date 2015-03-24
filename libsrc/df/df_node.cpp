///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2014 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of libsrc.
//
//    libsrc is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsrc is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsrc.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include "df_node.hpp"

Df_node::Df_node() : size(0), type(NONE), data_size(0), data_storage(0)
{

}

Df_node::~Df_node()
{
	if (type == BLOCK)
	{
		for (unsigned int i = 0; i < size; ++i)
			delete [] node[i];
		delete node;
	}
	else if (type ==  STRING)
	{
		for (unsigned int i = 0; i < size; ++i)
			delete [] cstr[i];
	}
	if (data_storage)
		free(data_storage);
}

Df_node const			*Df_node::get(std::string const &researched_nane) const
{
	unsigned long int	a;
	std::string			prefix;
	std::string			suffix;

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

Df_node const		*Df_node::safe_get(std::string const &researched_name, Type expected_type, unsigned int expected_size) const
{
	Df_node const	*rnode;

	if (!(rnode = get(researched_name)))
	{
		std::cerr << "error! Df_node::safe_get() fails to find: " << researched_name << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (rnode->type != expected_type)
	{
		std::cerr << "error! Df_node::safe_get() type doesn't match for " << researched_name << ": expected type " << expected_type << " actual type " << rnode->type << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (rnode->size < expected_size)
	{
		std::cerr << "error! Df_node::safe_get() size is too low for " << researched_name << ": expected size " << expected_size << " actual size " << rnode->size << std::endl;
		exit(EXIT_FAILURE);
	}
	return (rnode);
}

void	Df_node::print() const
{
	std::cout << name << ' ' << type << std::endl;
	if (type == BLOCK)
	{
		for (unsigned int i = 0; i < size; ++i)
			node[i]->print();
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
