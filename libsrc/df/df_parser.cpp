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

#include <list>
#include <cstdlib>
#include <cstring>
#include "../file/file.hpp"
#include "df_parser.hpp"

struct				Reference
{
	unsigned int	idx;
	char			name[64];
};

struct	Parser_state
{
	char const				*s;	//string
	unsigned int			i;	//iterator
	std::list<Df_node *>	ndlist;
	std::list<Reference>	reflist;
	unsigned int			size;
	char					*data;
	bool					error;
};

///////////////////////////////////////

static void	_add_name(std::list<Df_node *> &ndlist, std::string const &name)
{
	Df_node	*node = new Df_node();

	node->name = name;
	ndlist.push_back(node);
}

static void	_add_reference(Parser_state &ps, std::string const &reference)
{
	Df_node			*node;

	ps.size += ps.size % sizeof(void *) ? sizeof(void *) + sizeof(void *) - ps.size % sizeof(void *) : sizeof(void *);
	ps.data = (char *)realloc(ps.data, ps.size);
	node = ps.ndlist.back();
	if (node->type == Df_node::NONE)
	{
		node->type = Df_node::REFERENCE;
		node->size = 1;
		node->idx = ps.size - sizeof(void *);
	}
	else if (node->type != Df_node::REFERENCE)
	{
		node = new Df_node();
		node->size = 1;
		node->type = Df_node::REFERENCE;
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

static void			_add_data(Parser_state &ps, Df_node::Type const type, unsigned int const size, void const *data)
{
	Df_node			*node;

	ps.size += ps.size % size ? size + size - ps.size % size : size;
	ps.data = (char *)realloc(ps.data, ps.size);
	node = ps.ndlist.back();
	if (node->type == Df_node::NONE)
	{
		node->type = type;
		node->size = 1;
		node->idx = ps.size - size;
		memcpy(ps.data + node->idx, data, size);
	}
	else if (node->type != type)
	{
		node = new Df_node();
		node->size = 1;
		node->type = type;
		node->idx = ps.size - size;
		ps.ndlist.push_back(node);
		memcpy(ps.data + node->idx, data, size);
	}
	else
		memcpy(ps.data + (node->idx + node->size++ * size), data, size);
}

static void	_begin_block(std::list<Df_node *> &ndlist)
{
	Df_node	*node;

	node = ndlist.back();
	if (node->type != Df_node::NONE)
	{
		node = new Df_node();
		ndlist.push_back(node);
	}
	node->type = Df_node::BLOCK;
}

static void			_end_block(std::list<Df_node *> &ndlist)
{
	Df_node			*node = 0;
	unsigned int	size = 0;

	for (std::list<Df_node *>::reverse_iterator i = ndlist.rbegin(); i != ndlist.rend(); ++i)
	{
		if ((*i)->type == Df_node::BLOCK && (*i)->size == 0)
		{
			node = *i;
			break;
		}
		size++;
	}
	if (size > 1 || (size == 1 && ndlist.back()->name != ""))
	{
		node->type = Df_node::BLOCK;
		node->size = size;
		node->node = new Df_node*[size];
		for (unsigned int i = size; i > 0;)
		{
			node->node[--i] = ndlist.back();
			ndlist.pop_back();
		}
	}
	else if (size)
	{
		Df_node	*a = ndlist.back();

		ndlist.pop_back();
		node->type = a->type;
		node->size = a->size;
		node->idx = a->idx;
		delete a;
	}
}

static void	_set_ref(Parser_state &ps, Df_node *root)
{
	while (!ps.reflist.empty())
	{
		Reference	ref;

		ref = ps.reflist.front();
		ps.reflist.pop_front();
		*(Df_node const **)((char *)ps.data + ref.idx) = root->get(ref.name);
	}
}

static void	_set_ptr(Df_node *node, void *data)
{
	if (node->type == Df_node::BLOCK)
	{
		for (unsigned int i = 0; i < node->size; ++i)
			_set_ptr(node->node[i], data);
	}
	else
		node->data = ((char *)data + node->idx);
}

///////////////////////////////////////

static void	_skip_space(Parser_state &ps)
{
	while (ps.s[ps.i] == ' ' || ps.s[ps.i] == '\t' || ps.s[ps.i] == '\n' || ps.s[ps.i] == '\r')
		++ps.i;
}

static bool		_read_symbol(Parser_state &ps)
{
	std::string	str;

	_skip_space(ps);
	if ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_')
	{
		str += ps.s[ps.i++];
		while ((ps.s[ps.i] >= 'A' && ps.s[ps.i] <= 'Z') || (ps.s[ps.i] >= 'a' && ps.s[ps.i] <= 'z') || ps.s[ps.i] == '_' || (ps.s[ps.i] >= '0' && ps.s[ps.i] <= '9'))
			str += ps.s[ps.i++];

		_add_name(ps.ndlist, str);
		return (true);
	}
	else
		return (false);
}

static bool		_read_reference(Parser_state &ps)
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

static bool	_read_nbr(Parser_state &ps, std::string &str)
{
	if (ps.s[ps.i] < '0' || ps.s[ps.i] > '9')
		return (false);

	while (ps.s[ps.i] >= '0' && ps.s[ps.i] <= '9')
		str += ps.s[ps.i++];
	return (true);
}

static bool		_read_number(Parser_state &ps)
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
				_add_data(ps, Df_node::FLOAT, sizeof(float), &f);
				return (true);
			}
			else
			{
				double	d = atof(str.c_str());
				_add_data(ps, Df_node::DOUBLE, sizeof(double), &d);
				return (true);
			}
		}
		else
		{
			int	i = atoi(str.c_str());
			_add_data(ps, Df_node::INT, sizeof(int), &i);
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

static bool		_read_string(Parser_state &ps)
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

		char	*ptr = _strdup(str.c_str()); //TODO
		_add_data(ps, Df_node::STRING, sizeof(char *), &ptr);
		return (true);
	}
	return (false);
}

static bool	_read_open_bracket(Parser_state &ps)
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

static bool	_read_close_bracket(Parser_state &ps)
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

///////////////////////////////////////

static bool	_read_data(Parser_state &ps);

static bool	_read_xdata(Parser_state &ps)
{
	if (!_read_data(ps))
		return (false);
	while (_read_data(ps))
		;
	return (true);
}

static bool	_read_data(Parser_state &ps)
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

static bool	_read_def(Parser_state &ps);

static bool	_read_xexpr(Parser_state &ps)
{
	if (!_read_def(ps))
		return (false);
	while (_read_def(ps))
		;
	return (true);
}

static bool	_read_expr(Parser_state &ps)
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

static bool	_read_def(Parser_state &ps)
{
	if (_read_symbol(ps) && _read_expr(ps))
		return (true);
	else
		return (false);
}

///////////////////////////////////////

Df_node				*df_parse(const char *data)
{
	Df_node			*node;
	Parser_state	ps;

	node = 0;
	ps.s = data;
	ps.i = 0;
	ps.size = 0;
	ps.data = 0;
	ps.error = false;
	while (_read_def(ps));
	if (ps.error)
	{
		std::cerr << "error! df_parse() fails to parse: synthax error" << std::endl;
		free(ps.data);
	}
	else
	{
		node = new Df_node;
		node->size = (unsigned int)ps.ndlist.size();
		node->type = Df_node::BLOCK;
		node->name = "";
		node->node = new Df_node*[node->size];
		for (unsigned int i = 0; i < node->size; ++i)
		{
			node->node[i] = ps.ndlist.front();
			ps.ndlist.pop_front();
		}
		node->data_size = ps.size;
		node->data_storage = ps.data;
		_set_ref(ps, node);
		_set_ptr(node, ps.data);
	}
	return (node);
}

Df_node				*df_parse_file(char const *filename)
{
	char const	*str = read_file(filename, 0);

	if (str)
		return (df_parse(str));
	else
	{
		std::cerr << "error! df_parse_file() can't open file: " << filename << std::endl;
		return (0);
	}
}
