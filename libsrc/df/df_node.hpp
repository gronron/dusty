///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2012 Geoffrey TOURON (geoffrey.touron@gmail.com)
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

#ifndef DF_NODE_H_
#define DF_NODE_H_

#include <string>

class			Df_node
{
	public:
	
		enum	Type
		{
			NONE,
			BLOCK,
			INT,
			FLOAT,
			DOUBLE,
			STRING
		};


		unsigned int	size;
		Type			type;
		std::string		name;

		union
		{
			Df_node		**node;
			void		*data;
			int			idx;
			int 		*nbr;
			float		*flt;
			double		*dbl;
			char		**cstr;
		};
	
		unsigned int	data_size;
		char			*data_storage;	// allocated with malloc


		Df_node();
		~Df_node();

		Df_node const	*get(std::string const &researched_name) const;
		Df_node const	*safe_get(std::string const &researched_name, Type expected_type, unsigned int expected_size) const;

		void			print() const;
};

#endif
