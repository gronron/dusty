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

#pragma once

#include <string>
#include "../dynamic_array.hpp"

class	DFNode
{
public:

	static constexpr unsigned int const		NAMESIZE = 128;

	enum	Type : unsigned char
	{
		NONE,
		BLOCK,
		REFERENCE,
		INT,
		FLOAT,
		DOUBLE,
		STRING
	};


	unsigned int	size = 0;
	Type			type = NONE;
	std::string		user_type;
	std::string		name;

	union
	{
		DFNode		**node = nullptr;
		void		*data;
		int			idx;
		int 		*nbr;
		float		*flt;
		double		*dbl;
		char		**cstr;
	};


	DFNode() {}
	~DFNode();

	DFNode const *	get(std::string const & researched_name) const;
	DFNode const *	get(std::string const & researched_name, DFNode::Type const expected_type, unsigned int const expected_size, void * const out) const;
	DFNode const *	safe_get(std::string const & researched_name, DFNode::Type const expected_type, unsigned int const expected_size) const;

	void			print() const;
};

class	DFRoot
{
public:

	DynamicArray<DFNode *>	_roots;
	DynamicArray<DFNode>	_nodes;

	unsigned int			_data_size = 0;
	char *					_data_storage = nullptr;

	DFRoot();
	~DFRoot();

	DFNode const *	get(std::string const & researched_name) const;
	DFNode const *	get(std::string const & researched_name, DFNode::Type const expected_type, unsigned int const expected_size, void * const out) const;
	DFNode const *	safe_get(std::string const & researched_name, DFNode::Type const expected_type, unsigned int const expected_size) const;
};

class	DFUserType
{
public:

	explicit DFUserType() {}
	virtual ~DFUserType() {}

	virtual bool	check() = 0;
};

/*
template<class T>
T	*create(DFNode const * const node)
{
	T * const	instance = new T();
	instance.load(node);
	return (instance);
}

#define FACTORYREG(name) Factoryregister<name> const	reg_##name(CRC32(#name))



class	DFUserTypeManager
{
	public:

		using CreatePtr = void *(*)(DFNode const * const);

		struct	Class
		{
			unsigned int	hash;
			CF				cf;
		};


		static Factory	&get_instance();


		unsigned int	_classcount;
		unsigned int	_classsize;
		Class			*_classes;


		Factory();
		~Factory();

		void		register_class(unsigned int const, CF);

		Entity		*create(Gameengine *, Replication *) const;
		Entity		*create(Gameengine *, Replication *, int const, unsigned int const, Entity const *) const;
};

template<class T>
struct	Factoryregister
{
	Factoryregister(unsigned int const hash)
	{
		Factory::get_instance().register_class(hash, &create<T>);
	}
};
*/
