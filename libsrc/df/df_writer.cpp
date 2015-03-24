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

#include "df_writer.hpp"

bool	df_write(Df_node *node, std::ostream &os, unsigned int indent_size)
{
	os << std::string(indent_size, '\t') << node->name;
	if (node->type == Df_node::BLOCK)
	{
		os << '\n' << std::string(indent_size, '\t') << "{\n";
		for (unsigned int i = 0; i < node->size; ++i)
			df_write(node->node + i, os, indent_size + 1);
		os << std::string(indent, '\t') << "}\n";
	}
	else if (node->type == Df_node::INT)
	{
		if (node->size > 1)
		{
			os << "\t{ ";
			for (unsigned int i = 0; i < node->size; ++i)
				os << node->nbr[i] << ' ';
			os << '}';
		}
		else
			os << node->nbr[0];
	}
	else if (node->type == Df_node::FLOAT)
	{
		for (unsigned int i = 0; i < size; ++i)
			std::cout << node->flt[i] << ' ';
	}
	else if (node->type == Df_node::DOUBLE)
	{
		for (unsigned int i = 0; i < size; ++i)
			std::cout << node->dbl[i] << ' ';
	}
	else if (node->type == Df_node::STRING)
	{
		for (unsigned int i = 0; i < size; ++i)
			std::cout << node->cstr[i] << ' ';
	}
	else
		; //error
	os << '\n';
	return (true);
}
