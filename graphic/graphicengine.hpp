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

#ifndef GRAPHICENGINE_H_
#define GRAPHICENGINE_H_

#include <string>
#include <map>
#include <list>
#include "math/vec.hpp"
#include "animation.hpp"

namespace sf
{
	class Window;
}

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

		sf::Window						*_window;

		vec<unsigned int, 2>			screensize;
		std::string						assetsdir;
		vec<float, 2>					*cam;

		Glyph							_glyphtex[128];
		unsigned int					_textcoord;
		unsigned int					_activetex;
		std::map<std::string, Texture>	_texmap;
		std::list<Animation *>			_animationlist;
		std::list<Text *>				_textlist;
		vec<float, 2>					_top;
		vec<float, 2>					_bot;


		Graphicengine(vec<unsigned int, 2> const &, std::string const &);
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
