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

#ifndef GRAPHICENGINE_H_
#define GRAPHICENGINE_H_

#include <string>
#include <map>
#include <list>
#include "SDL.h"
#include "math/vec.hpp"
#include "animation.hpp"

class	Graphicengine
{
	public:

		struct	Texture
		{
			unsigned int			id;
			vec<unsigned int, 2>	size;
		};
		
		struct	Text
		{
			std::string		text;
			vec<float, 2>	trans;
			vec<float, 2>	scale;
			vec<float, 4>	color;
		};
		
		struct	Glyph : public Texture
		{
			vec<float, 2>	step;
			vec<float, 2>	center;
		};
		
		std::string						assetsdir;
		vec<unsigned int, 2>			screensize;
		vec<float, 3>					cam_loc;
		vec<float, 3>					cam_dir;

		SDL_Window		*_window;
		SDL_GLContext	_glcontext;

		Glyph							_glyphtex[128];
		unsigned int					_textcoord;
		unsigned int					_activetex;
		std::map<std::string, Texture>	_texmap;
		std::list<Animation *>			_animationlist;
		std::list<Text *>				_textlist;
		vec<float, 2>					_top;
		vec<float, 2>					_bot;


		Graphicengine();
		~Graphicengine();

		void	add(Animation *);
		void	remove(Animation *);

		void	tick(float);

		void	resize(unsigned int, unsigned int);

		Texture	*load_texture(std::string const &);
		void	draw_texture(Texture const *, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &);
		void	draw_text(std::string const &, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &);

		void	_load_glyph();
		void	_draw_text(Text const *);
};

#endif
