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

#ifndef CONSOLE_H_
#define CONSOLE_H_

#define MAXCHARACTER 1024
#define MAXHISTORY 1024

class	Gameengine;

class	Console
{
	public:

		enum	Cursormove { UP, DOWN, LEFT, RIGHT };
		
		struct	Text
		{
			char			text[MAXCHARACTER];
			unsigned int	size;
			unsigned int	linecount;
			unsigned short int	start[8];
			unsigned short int	stop[8];
		};


		Gameengine		*engine;

		unsigned int	_textsize;
		unsigned int	_cursor;
		bool			_blink;
		float			_blinktimer;
		float			_lastmsgtimer;
		int				_iterator;
		Text			_history[MAXHISTORY];
		char			_text[MAXCHARACTER];


		Console(Gameengine *);
		~Console();
		
		void	tick(float const);
		void	draw();
		
		void	put_text(char const *);
		void	put_char(char const);
		void	move_cursor(char const);
		
		void	_cut_text(Text &);
};

#endif
