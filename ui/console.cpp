///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2012 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of Chasis.
//
//    Chasis is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Chasis is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Chasis.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#include "actormanager.hpp"
#include "eventmanager.hpp"
#include "networkengine.hpp"
#include "graphicengine.hpp"
#include "console.hpp"

Console::Console(Actormanager *a) : am(a), _blink(false), _blinktime(0.0f), _cursor(0), _text("")
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
	loc = vec<float, 2>::cast(am->ge->screensize) / -2.0f;
	loc[1] += _history.size() < 4 ? _history.size() * 32.0f : 128.0f;
	s = 0.5f;
	if (am->em->typing)
	{
		text = _text;
		if (_blink)
			text.replace(_cursor, 1, 1, '_');
		//am->ge->draw_text(text, loc + *am->ge->cam, s, color);
	}
	j = 0;
	for (i = _history.rbegin(); i != _history.rend() && j < 4; ++i)
	{
		loc[1] -= 32;
		j++;
		//am->ge->draw_text(*i, loc + *am->ge->cam, s, color);
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
			if (am->master)
				puttext(_text);
			else
				am->ne->sendtextmsg(_text);
			_text.clear();
		}
		am->em->toogletyping();
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
	if (!am->local && am->master)
		am->ne->sendtextmsg(str);
	_history.push_back(str);
}
