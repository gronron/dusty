/******************************************************************************
Copyright (c) 2015-2016, Geoffrey TOURON
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

#include "gameengine.hpp"
#include "eventmanager.hpp"
#include "networkengine.hpp"
#include "graphicengine.hpp"
#include "console.hpp"

#include <iostream>

Console::Console(Gameengine *e) : engine(e), _textsize(0), _cursor(0), _blink(false), _blinktimer(0.0f), _lastmsgtimer(0.0f)
{
	for (unsigned int i = 0; i < MAXHISTORY; ++i)
	{
		_history[i].text[0] = '\0';
		_history[i].size = 0;
	}
	_textsize = 4;
	strcpy(_text, "hola");
}

Console::~Console()
{

}

void	Console::tick(float const delta)
{
	if ((_blinktimer += delta) >= 0.5f)
	{
		_blinktimer -= 0.5f;
		_blink = !_blink;
	}
	if (_lastmsgtimer >= 0.0f)
		_lastmsgtimer -= delta;
}

void					Console::draw()
{
	vec<float, 2>		position = { 8.0f, 8.0f };
	vec<float, 2> const	scale = { 0.4f, 0.4f };
	vec<float, 4> const	color = { 0.4f, 0.7f, 0.4f, 1.0f };

	/*if (_lastmsgtimer >= 0.0f)
	{
		for (unsigned int i = (_iterator - 8) % MAXHISTORY; i != _iterator; i = (i + 1) % MAXHISTORY)
		{
			engine->graphic->draw_text(_history[i].text, position, scale, color);
			position[1] -= 32.0f;
		}
	}*/
	if (_textsize)
	{
		char const	c = _text[_cursor];

		if (_blink)
			_text[_cursor] = '_';
		engine->graphic->draw_text(_text, position, scale, color);
		_text[_cursor] = c;
	}
}

void	Console::put_text(char const *str)
{
	_lastmsgtimer = 5.0f;
	strcpy(_history[_iterator].text, str);
	_iterator = (_iterator + 1 ) % MAXHISTORY;
}

void	Console::put_char(char const c)
{
	if (c == '\n')
	{
		if (_textsize)
		{
			if (engine->master)
				put_text(_text);
			else
				engine->network->send_textmsg(_text);
			_textsize = 0;
			_cursor = 0;
		}
	}
	else if (c == '\b')
	{
		if (_cursor > 0)
		{
			--_textsize;
			for (unsigned int i = --_cursor; _text[i]; ++i)
				_text[i] = _text[i + 1];
		}
	}
	else if (c == 0x7f)
	{
		if (_cursor < _textsize)
		{
			--_textsize;
			for (unsigned int i = _cursor; _text[i]; ++i)
				_text[i] = _text[i + 1];
		}
	}
	else
	{
		for (unsigned int i = _textsize; i > _cursor; --i)
			_text[i] = _text[i - 1];
		_text[_cursor++] = c;
		_text[++_textsize] = '\0';
		_text[_textsize + 1] = '\0';
	}
}

void	Console::move_cursor(char const move)
{
	switch (move)
	{
		case UP:
			_cursor = 0;
			break;
		case DOWN:
			_cursor = _textsize;
			break;
		case LEFT:
			if (_cursor > 0)
				--_cursor;
			break;
		case RIGHT:
			if (_cursor < _textsize)
				++_cursor;
			break;
	}
}
