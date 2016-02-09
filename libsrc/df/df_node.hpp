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
			REFERENCE,
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
		Df_node	const	*get(std::string const &researched_name, Type expected_type, unsigned int expected_size, void *out) const;
		Df_node const	*safe_get(std::string const &researched_name, Type expected_type, unsigned int expected_size) const;

		void			print() const;
};

#endif
