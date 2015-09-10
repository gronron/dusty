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

#include "gameengine.hpp"
#include "eventmanager.hpp"
#include "networkengine.hpp"
#include "graphicengine.hpp"
#include "console.hpp"

Console::Console(Gameengine *g) : engine(g), _blink(false), _blinktime(0.0f), _cursor(0), _text("")
{

}

Console::~Console()
{

}

void	Console::tick(float delta)
{
	_blinktime += delta;
	if (_blinktime >= 0.5f)
	{
		_blinktime -= 0.5f;
		_blink = !_blink;
	}
}

void											Console::draw()
{
	std::list<std::string>::reverse_iterator	i;
	vec<float, 2>								loc;
	vec<float, 2>								s;
	vec<float, 4>								color;
	std::string									text;
	int											j;

	color = 1.0f;
	loc = vec<float, 2>::cast(engine->graphic->screensize) / -2.0f;
	loc[1] += _history.size() < 4 ? _history.size() * 32.0f : 128.0f;
	s = 0.5f;
	if (engine->event->typing)
	{
		text = _text;
		if (_blink)
			text.replace(_cursor, 1, 1, '_');
		//engine->ge->draw_text(text, loc + *engine->ge->cam, s, color);
	}
	j = 0;
	for (i = _history.rbegin(); i != _history.rend() && j < 4; ++i)
	{
		loc[1] -= 32;
		j++;
		//engine->ge->draw_text(*i, loc + *engine->ge->cam, s, color);
	}
}

void	Console::movecursor(Cursormove a)
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

void	Console::putchar(char c)
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

void	Console::puttext(std::string const &str)
{
	if (engine->network && engine->master)
		engine->network->sendtextmsg(str);
	_history.push_back(str);
}
