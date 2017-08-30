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
#include "new.hpp"
#include "gameengine.hpp"
#include "interpreter.hpp"

Interpreter::Interpreter(Gameengine *e) : engine(e), _cmdcount(0), _cmdsize(256), _commands(0)
{
	_commands = new Command[_cmdsize];
	_register_all();
}

Interpreter::~Interpreter()
{
	delete [] _commands;
}

void	Interpreter::register_function(char const *name, char const *description, void (*function)(Gameengine *, Argument *), Argumenttypelist const &atl)
{
	if (_cmdcount >= _cmdsize)
	{
		_cmdsize <<= 1;
		_commands = resize(_commands, _cmdcount, _cmdsize);
	}
	
	unsigned int	argcount;
	for (argcount = 0; atl.argtypes[argcount] != NONE; ++argcount);

	unsigned int	i;
	for (i = _cmdcount++; i && strcmp(name, _commands[i - 1].name) < 0; --i)
		_commands[i] = _commands[i - 1];

	_commands[i].name = name;
	_commands[i].description = description;
	_commands[i].function = function;
	_commands[i].argcount = argcount;
	for (unsigned int j = 0; j < argcount; ++j)
		_commands[i].argtypes[j] = atl.argtypes[j];
}

void				Interpreter::exec(char const *str)
{
	unsigned int	cursor = 0;
	char			name[MAXWORDSIZE];
	unsigned int	argcount = 0;
	Argument		args[16];

	if (_read_symbol(str, cursor, name))
	{
		while (_read_number(str, cursor, argcount, args) || _read_string(str, cursor, argcount, args));

		int const	cmdindex = _find_commandindex(name, argcount, args);
		if (cmdindex >= 0)
			(_commands[cmdindex].function)(engine, args);
	}
	else
		std::cout << "there is no command starting like this " << str << std::endl;
}

int		Interpreter::_find_commandindex(char const *name, unsigned int const argcount, Argument const *args)
{
	int	idx = -1;
	int	imin = 0;
	int	imax = _cmdcount - 1;

	while (imin <= imax)
	{
		int const	imid = (imin + imax) >> 1;
		int const	cmp = strcmp(name, _commands[imid].name);

		if (cmp < 0)
			imax = imid - 1;
		else if (cmp > 0)
			imin = imid + 1;
		else
		{
			for (idx = imid; idx >= 0 && !strcmp(name, _commands[idx].name); --idx);
			++idx;
			break;
		}
	}

	if (idx >= 0)
	{
		for (;idx < (int)_cmdcount && !strcmp(name, _commands[idx].name); ++idx)
		{
			if (argcount != _commands[idx].argcount)
				continue;

			unsigned int i = -1;
			do
				if (++i == argcount)
					return (idx);
			while (i < argcount && args[i].type == _commands[idx].argtypes[i]);
		}
		std::cout << "there is no function called \"" << name << "\" matching the argument" << std::endl;
	}
	else
		std::cout << "not found" << std::endl;
	return (-1);
}

static void	_skip_space(char const *s, unsigned int &i)
{
	while (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')
		++i;
}

bool				Interpreter::_read_symbol(char const *s, unsigned int &i, char *name)
{
	unsigned int	j = 0;

	_skip_space(s, i);

	if ((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z') || s[i] == '_')
	{
		name[j++] = s[i++];
		while ((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z') || s[i] == '_' || (s[i] >= '0' && s[i] <= '9'))
		{
			name[j++] = s[i++];
			if (j >= MAXWORDSIZE)
				;//engine->put_text("symbol too long");
		}
		name[j] = '\0';
		return (true);
	}
	else
		return (false);
}

bool	Interpreter::_read_nbr(char const *s, unsigned int &i, char *str, unsigned int &j)
{
	if (s[i] < '0' || s[i] > '9')
		return (false);
	
	while (s[i] >= '0' && s[i] <= '9')
	{
		str[j++] = s[i++];
		if (j >= MAXWORDSIZE)
			;//engine->put_text("number too long");
	}
	str[j] = '\0';
	return (true);
}

bool				Interpreter::_read_number(char const *s, unsigned int &i, unsigned int &index, Argument *args)
{
	char			str[MAXWORDSIZE]; //buffer overflow
	unsigned int	j = 0;
	
	_skip_space(s, i);

	if (s[i] == '+' || s[i] == '-')
		str[j++] = s[i++];

	if (_read_nbr(s, i, str, j))
	{
		if (s[i] == '.')
		{
			str[j++] = s[i++];
			_read_nbr(s, i, str, j);
			args[index].type = DOUBLE;
			args[index++].dbl = atof(str);
		}
		else
		{
			args[index].type = INTEGER;
			args[index++].nbr = atoi(str);
		}
		return (true);
	}
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

bool				Interpreter::_read_string(char const *s, unsigned int &i, unsigned int &index, Argument *args)
{
	unsigned int	j = 0;
	
	_skip_space(s, i);

	if (s[i] == '"')
	{
		++i;
		while (s[i] && s[i] != '"')
		{
			if (s[i] == '\\')
			{
				if (!s[++i])
					return (false);
				args[index].str[j++] = _to_c_char(s[i++]);
			}
			else
				args[index].str[j++] = s[i++];
			if (j >= MAXSTRINGSIZE)
				;//engine->put_text("number too long");
		}
		if (s[i] == '"')
			++i;
		args[index].str[j] = '\0';
		args[index++].type = STRING;
		return (true);
	}
	else
		return (false);
}
