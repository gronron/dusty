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

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string>
#include <list>

class	Actormanager;

class	Console
{
	public:

		enum	Cursormove { UP, DOWN, LEFT, RIGHT };

		struct			Recentmessage
		{
			float		time;
			std::string	msg;
		};


		Actormanager	*am;

		bool		_blink;
		float		_blinktime;
		int			_cursor;
		std::string	_text;

		std::list<std::string>		_history;
		std::list<Recentmessage>	_recentmsg;


		Console(Actormanager *);
		~Console();

		void	tick(float);
		void	draw();

		void	movecursor(Cursormove);
		void	putchar(char);
		void	puttext(std::string const &);
};

#endif
