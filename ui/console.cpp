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

#if defined(_WIN32) || defined(__WIN32__)
	#include <SDKDDKVer.h>
	#include <winsock2.h>
	#include <io.h>
#else
	#include <sys/select.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif

#include <stdio.h>
#include "gameengine.hpp"

#include "eventmanager.hpp"
#include "networkengine.hpp"
#include "graphicengine.hpp"
#include "console.hpp"

Console::Console(Gameengine *e) : engine(e)
{

}

Console::~Console()
{

}

void				Console::tick(float const delta)
{
	fd_set			readfds;
	struct timeval	tv = { 0, 0 };
	int				ret;
/*
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);

	if ((ret = select(1, &readfds, 0, 0, &tv)) < 0)
	{
		perror("Error! select psst");
		exit(EXIT_FAILURE);
	}
	else if (ret == 1 && FD_ISSET(0, &readfds))
	{
		char		str[65536];
#if defined(_WIN32) || defined(__WIN32__)
		int	const	size = _read(0, str, 65536);
#else
		int	const	size = read(0, str, 65536);
#endif

		if (size > 0)
		{
			str[size] = '\0';
			if (engine->network)
				engine->network->send_textmsg(str);
		}
	}*/
}

void	Console::put_text(char const *str)
{
#if defined(_WIN32) || defined(__WIN32__)
	_write(1, str, strlen(str));
	_write(1, "\n", 1);
#else
	write(1, str, strlen(str));
	write(1, "\n", 1);
#endif
}

///////////////////////////////////////////////////////////////////////////////

Gameconsole::Gameconsole(Gameengine *e) : Console(e), _cursor(0), _blink(false), _blinktimer(0.0f), _lastmsgtimer(0.0f)
{
	/*for (unsigned int i = 0; i < MAXHISTORY; ++i)
	{
		*_lines[i].text = '\0';
		_lines[i].size = 0;
	}*/
}

Gameconsole::~Gameconsole()
{

}

void	Gameconsole::tick(float const delta)
{
	/*if ((_blinktimer += delta_ >= 0.5f)
	{
		_blinktimer -= 0.5f;
		_blink = !_blink;
	}
	if (_lastmsgtimer >= 0.0f)
		_lastmsgtimer -= delta;*/
}

void					Gameconsole::draw()
{
	/*vec<float, 2>		position = 8.0f;
	vec<float, 2> const	scale = 0.0f;
	vec<float, 4> const	color = 1.0f;

	if (_lastmsgtimer >= 0.0f)
	{
		for (unsigned int i = (_iterator - 8) % MAXHISTORY; i != _iterator; i = (i + 1) % MAXHISTORY)
		{
			engine->graphic->draw_text(_lines[i].text, position, scale, color);
			position[1] -= 32.0f;
		}
	}
	if (_text[0])
	{
		char	str[MAXCHARACTER];
		
		strcpy(str, _text);
		if (_blink)
			str[_cursor] = '_';
		engine->graphic->draw_text(str, position, scale, color);
	}*/
}

void	Gameconsole::put_text(char const *str)
{
	/*strcpy(_lines[_iterator].text, str);
	_iterator = (_iterator + 1 ) % MAXHISTORY;
	if (engine->network && engine->master)
		engine->network->sendtextmsg(str);*/
	
}

void	Gameconsole::update_text(char const *str, unsigned int cursor)
{
	/*_cursor = cursor;
	strcpy(_text, str); */
}
/*
void	Gameconsole::put_char(char const c)
{
	if (c == '\r' || c == '\n')
	{
		if (_text.length() != 0)
		{
			if (engine->master)
				puttext(_text);
			else
				engine->network->sendtextmsg(_text);
			_text.clear();
		}
		engine->event->toogletyping();
		_cursor = 0;
	}
	else if (c == '\b')
	{
		if (_cursor > 0)
			_text.erase(--_cursor, 1);
	}
	else if (c == 0x7f)
		_text.erase(_cursor, 1);
	else
		_text.insert(_cursor++, 1, c);
}

void	Gameconsole::move_cursor(Cursormove const a)
{
	if (a == UP)
		_cursor = 0;
	else if (a == DOWN)
		_cursor = _text.size();
	else if (a == LEFT)
	{
		if (_cursor > 0)
			--_cursor;
	}
	else if (a == RIGHT)
	{
		if (_cursor < _text.size())
			++_cursor;
	}
}
*/
