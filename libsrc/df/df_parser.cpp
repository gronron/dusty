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

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <list>
#include "../file/file.hpp"
#include "df_parser.hpp"

struct				Reference
{
	unsigned int	idx;
	char			name[64];
};

struct						ParserState
{
	DFRoot *				root;
	char const *			s;	//string
	unsigned int			i;	//iterator
	std::list<DFNode *>		ndlist;
	std::list<Reference>	reflist;
	bool					error;
};

///////////////////////////////////////

static void	_add_name(ParserState & ps, std::string const & name)
{
	DFNode	*node = ps.root->_nodes.allocate();

	node->name = name;
	ps.ndlist.push_back(node);
}

static void _add_user_type(ParserState & ps, std::string const & user_type)
{
	DFNode * const	node = ps.ndlist.back();

	node->user_type = user_type;
}

static void			_add_reference(ParserState & ps, std::string const & reference)
{
	DFNode * const node = ps.ndlist.back();

	ps.root->_data_size += ps.root->_data_size % sizeof(void *) ? sizeof(void *) + sizeof(void *) - ps.root->_data_size % sizeof(void *) : sizeof(void *);
	ps.root->_data_storage = (char *)realloc(ps.root->_data_storage, ps.root->_data_size);

	if (node->type == DFNode::NONE)
	{
		node->type = DFNode::REFERENCE;
		node->size = 1;
		node->idx = ps.size - sizeof(void *);
	}
	else if (node->type != DFNode::REFERENCE)
	{
		node = ps.root->_nodes.allocate();
		node->size = 1;
		node->type = DFNode::REFERENCE;
		node->idx = ps.size - sizeof(void *);
		ps.ndlist.push_back(node);
	}
	else
		++node->size;

	Reference	ref;
	ref.idx = node->idx + (node->size - 1) * sizeof(void *);
	strcpy_s(ref.name, sizeof(ref.name), reference.c_str());
	ps.reflist.push_back(ref);
}

static void			_add_data(ParserState & ps, DFNode::Type const type, unsigned int const size, void const * const data)
{
	DFNode * const	node = ps.ndlist.back();

	ps.root->_data_size += ps.root->_data_size % size ? size + size - ps.root->_data_size % size : size;
	ps.root->_data_storage = (char *)realloc(ps.root->_data_storage, ps.root->_data_size);

	if (node->type == DFNode::NONE)
	{
		node->type = type;
		node->size = 1;
		node->idx = ps.size - size;
		memcpy(ps.data + node->idx, data, size);
	}
	else if (node->type != type)
	{
		node = ps.root->_nodes.allocate();
		node->size = 1;
		node->type = type;
		node->idx = ps.size - size;
		ps.ndlist.push_back(node);
		memcpy(ps.data + node->idx, data, size);
	}
	else
		memcpy(ps.data + (node->idx + node->size++ * size), data, size);
}

static void		_begin_block(std::list<DFNode *> & ndlist)
{
	DFNode *	node = ndlist.back();

	if (node->type != DFNode::NONE)
	{
		node = ps.root->_nodes.allocate();
		ndlist.push_back(node);
	}
	node->type = DFNode::BLOCK;
}

static void			_end_block(std::list<DFNode *> & ndlist)
{
	DFNode *		node = nullptr;
	unsigned int	size = 0;

	for (std::list<DFNode *>::reverse_iterator i = ndlist.rbegin(); i != ndlist.rend(); ++i)
	{
		if ((*i)->type == DFNode::BLOCK && (*i)->size == 0)
		{
			node = *i;
			break;
		}
		size++;
	}
	if (size > 1 || (size == 1 && ndlist.back()->name != ""))
	{
		node->type = DFNode::BLOCK;
		node->size = size;
		node->node = new DFNode*[size];
		for (unsigned int i = size; i > 0;)
		{
			node->node[--i] = ndlist.back();
			ndlist.pop_back();
		}
	}
	else if (size)
	{
		DFNode * const a = ndlist.back();

		ndlist.pop_back();
		node->type = a->type;
		node->size = a->size;
		node->idx = a->idx;
		delete a;
	}
}

static void	_set_ref(ParserState & ps, DFNode * const node)
{
	while (!ps.reflist.empty())
	{
		Reference	ref = ps.reflist.front();

		ps.reflist.pop_front();
		*(DFNode const **)((char *)ps.data + ref.idx) = node->get(ref.name);
	}
}

static void	_set_ptr(DFNode * const node, void * data)
{
	if (node->type == DFNode::BLOCK)
	{
		for (unsigned int i = 0; i < node->size; ++i)
			_set_ptr(node->node[i], data);
	}
	else
		node->data = ((char *)data + node->idx);
}

///////////////////////////////////////

static void	_skip_space(ParserState & ps)
{
	while (ps.s[ps.i] == ' ' || ps.s[ps.i] == '\t' || ps.s[ps.i] == '\n' || ps.s[ps.i] == '\r')
		++ps.i;
}

static bool		_read_symbol(ParserState &ps)
{
	std::string	str;

	_skip_space(ps);
	if ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_')
	{
		str += ps.s[ps.i++];
		while ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_' || (ps.s[ps.i] >= '0' && ps.s[ps.i] <= '9'))
			str += ps.s[ps.i++];

		_add_name(ps, str);
		return (true);
	}
	else
		return (false);
}

static bool		_try_read_user_type(ParserState & ps)
{
	std::string	str;

	_skip_space(ps);
	if (ps.s[ps.i] == ':')
	{
		++ps.i;
		_skip_space(ps);

		if ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_')
		{
			str += ps.s[ps.i++];
			while ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_' || (ps.s[ps.i] >= '0' && ps.s[ps.i] <= '9'))
				str += ps.s[ps.i++];

			_add_user_type(ps.ndlist, str);
			return (true);
		}
		else
		{
			ps.error = true;
			return (false);
		}
	}
	else
		return (true);
}

static bool		_read_reference(ParserState & ps)
{
	std::string	str;

	_skip_space(ps);
	if (ps.s[ps.i] == '@')
	{
		++ps.i;
		if ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_')
		{
			str += ps.s[ps.i++];
			while ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_' || (ps.s[ps.i] >= '0' && ps.s[ps.i] <= '9'))
				str += ps.s[ps.i++];

			_add_reference(ps, str);
			return (true);
		}
		else
		{
			ps.error = true;
			return (false);
		}
	}
	else
		return (false);
}

static bool	_read_nbr(ParserState & ps, std::string & str)
{
	if (ps.s[ps.i] < '0' || ps.s[ps.i] > '9')
		return (false);

	while (ps.s[ps.i] >= '0' && ps.s[ps.i] <= '9')
		str += ps.s[ps.i++];
	return (true);
}

static bool		_read_number(ParserState & ps)
{
	std::string	str;

	_skip_space(ps);
	if (ps.s[ps.i] == '+' || ps.s[ps.i] == '-')
		str += ps.s[ps.i++];
	if (_read_nbr(ps, str))
	{
		if (ps.s[ps.i] == '.')
		{
			str += ps.s[ps.i++];
			_read_nbr(ps, str);

			if (ps.s[ps.i] == 'F' || ps.s[ps.i] == 'f')
			{
				++ps.i;
				float	f = (float)atof(str.c_str());
				_add_data(ps, DFNode::FLOAT, sizeof(float), &f);
				return (true);
			}
			else
			{
				double	d = atof(str.c_str());
				_add_data(ps, DFNode::DOUBLE, sizeof(double), &d);
				return (true);
			}
		}
		else
		{
			int	i = atoi(str.c_str());
			_add_data(ps, DFNode::INT, sizeof(int), &i);
			return (true);
		}
	}
	else
		return (false);
}

static char				_to_c_char(char const c)
{
	static char const	*c_char = "a\ab\bt\tn\nv\vf\fr\r0\0";

	for (unsigned int i = 0; c_char[i]; i += 2)
	{
		if (c_char[i] == c)
			return (c_char[i + 1]);
	}
	return (c);
}

static bool		_read_string(ParserState & ps)
{
	std::string	str;

	_skip_space(ps);
	if (ps.s[ps.i] == '"')
	{
		++ps.i;
		while (ps.s[ps.i] != '"' && ps.s[ps.i])
		{
			if (ps.s[ps.i] == '\\')
			{
				if (!ps.s[++ps.i])
				{
					ps.error = true;
					return (false);
				}
				str += _to_c_char(ps.s[ps.i++]);
			}
			else
				str += ps.s[ps.i++];
		}

		char *	ptr = _strdup(str.c_str()); //TODO
		_add_data(ps, DFNode::STRING, sizeof(char *), &ptr);
		return (true);
	}
	return (false);
}

static bool	_read_open_bracket(ParserState & ps)
{
	_skip_space(ps);
	if (ps.s[ps.i] == '{')
	{
		++ps.i;
		_begin_block(ps.ndlist);
		return (true);
	}
	else
		return (false);
}

static bool	_read_close_bracket(ParserState & ps)
{
	_skip_space(ps);
	if (ps.s[ps.i] == '}')
	{
		++ps.i;
		_end_block(ps.ndlist);
		return (true);
	}
	else
		return (false);
}

static bool	_read_colon(ParserState & ps)
{
	_skip_space(ps);
	if (ps.s[ps.i] == ':')
	{
		++ps.i;
		return (true);
	}
	else
		return (false);
}

///////////////////////////////////////

static bool	_read_data(ParserState & ps);

static bool	_read_xdata(ParserState & ps)
{
	if (!_read_data(ps))
		return (false);
	while (_read_data(ps))
		;
	return (true);
}

static bool	_read_data(ParserState & ps)
{
	if (_read_open_bracket(ps) && _read_xdata(ps) && _read_close_bracket(ps))
		return (true);
	else if (_read_reference(ps))
		return (true);
	else if (_read_number(ps))
		return (true);
	else if (_read_string(ps))
		return (true);
	else
		return (false);
}

static bool	_read_def(ParserState & ps);

static bool	_read_xexpr(ParserState & ps)
{
	if (!_read_def(ps))
		return (false);
	while (_read_def(ps))
		;
	return (true);
}

static bool	_read_expr(ParserState & ps)
{
	if (_read_open_bracket(ps))
	{
		if (_read_xdata(ps) && _read_close_bracket(ps))
			return (true);
		else if (_read_xexpr(ps) && _read_close_bracket(ps))
			return (true);
		else
			return (false);
	}
	else if (_read_data(ps))
		return (true);
	else
	{
		ps.error = true;
		return (false);
	}
}

static bool	_read_def(ParserState & ps)
{
	if (_read_symbol(ps) && _try_read_user_type(ps) && _read_expr(ps))
		return (true);
	else
		return (false);
}

///////////////////////////////////////

bool			df_parse(DFRoot & root, const char * const data)
{
	DFNode *	node;
	ParserState	ps;

	node = 0;
	ps.root = &root;
	ps.s = data;
	ps.i = 0;
	ps.error = false;
	ps.root = &root;

	while (_read_def(ps));

	if (ps.error)
	{
		std::cerr << "error! df_parse() fails to parse: synthax error" << std::endl;
		free(ps.data);
		return (false);
	}
	else
	{
		DFNode * const node = root->_nodes.allocate();
		node->size = (unsigned int)ps.ndlist.size();
		node->type = DFNode::BLOCK;
		node->name = "";
		node->node = new DFNode*[node->size];

		for (unsigned int i = 0; i < node->size; ++i)
		{
			node->node[i] = ps.ndlist.front();
			ps.ndlist.pop_front();
		}

		_set_ref(ps, node);
		_set_ptr(node, root._data_storage);

		root._root.allocate() = node;

		return (true);
	}
}

DFRoot *	df_parse_file(DFRoot & root, char const * const file_name)
{
	char const * const	str = read_file(file_name, 0);

	if (str)
	{
		df_parse(root, str);
		delete [] str;
		return (root);
	}
	else
	{
		std::cerr << "error! df_parse_file() can't open file: " << file_name << std::endl;
		return (nullptr);
	}
}

DFRoot *	df_parse_dir(DFRoot & root, char const * const dir_name)
{
	for (auto const & file : std::filesystem::directory_iterator(dir_name))
	{
		if (file.path().extension().string() == ".df")
		{
			char const * const	str = read_file(file.path().string().c_str(), 0);

			if (str)
			{
				df_parse(root, str);
				delete [] str;
			}
			else
			{
				std::cerr << "error! df_parse_file() can't open file: " << file_name << std::endl;
				delete [] str;
				return (nullptr);
			}
		}
	}
}
