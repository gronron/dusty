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

#include <iostream>
#include <cstdlib>
#include "image.hpp"
#include "graphicengine.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <SFML/Window.hpp>

Graphicengine::Graphicengine(vec<unsigned int, 2> const &ss, std::string const &ad) : _window(), screensize(ss), assetsdir(ad), cam(0), _activetex(0)
{
	GLenum	error;

	_window = new sf::Window(sf::VideoMode(screensize[0], screensize[1], 32), "Chasis", sf::Style::Default, sf::ContextSettings(0, 0, 0, 3, 3));

	if ((error = glewInit()) != GLEW_OK)
	{
		std::cerr << "Error: glewInit()" << glewGetErrorString(error) << std::endl;
		exit(EXIT_FAILURE);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	
	_load_glyph();

	glViewport(0, 0, screensize[0], screensize[1]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, screensize[0], screensize[1], 0.0, -1.0, 1.0);
}

Graphicengine::~Graphicengine()
{
	std::list<Animation *>::iterator	i;

	for (i = _animationlist.begin(); i != _animationlist.end(); ++i)
		delete *i;
	delete _window;
}

void	Graphicengine::add(Animation *a)
{
	std::list<Animation *>::iterator	i;

	for (i = _animationlist.begin(); i != _animationlist.end() && a->depth >= (*i)->depth; ++i);
	if (i != _animationlist.end())
		_animationlist.insert(i, a);
	else
		_animationlist.push_back(a);
}

void	Graphicengine::remove(Animation *a)
{
	_animationlist.remove(a);
}

void									Graphicengine::tick(float delta)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	_top = vec<float, 2>::cast(screensize) / -2.0f;
	_bot = -_top;
	if (cam)
	{
		_top += *cam;
		_bot += *cam;
	}
	glTranslatef(-_top[0], -_top[1], 0.0f);
	for (std::list<Animation *>::iterator i = _animationlist.begin(); i != _animationlist.end();)
	{
		if (!(*i)->tick(delta))
		{
			delete *i;
			i = _animationlist.erase(i);
		}
		else
		{
			(*i)->draw();
			++i;
		}
	}
	for (std::list<Text *>::iterator i = _textlist.begin(); i != _textlist.end();)
	{
		_draw_text(*i);
		delete *i;
		i = _textlist.erase(i);
	}
	_window->display();
}

void	Graphicengine::resize(unsigned int w, unsigned int h)
{
	screensize[0] = w;
	screensize[1] = h;

	glViewport(0, 0, screensize[0], screensize[1]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, screensize[0], screensize[1], 0.0, -1.0, 1.0);

}

Graphicengine::Texture							*Graphicengine::load_texture(std::string const &name)
{
	std::map<std::string, Texture>::iterator	i;
	GLenum										format = GL_RGBA;

	if ((i = _texmap.find(name)) != _texmap.end())
		return (&i->second);
    else
    {
    	Texture		texture;
    	Image		img(assetsdir + name);

    	if (!img.data)
			exit(EXIT_FAILURE);

		texture.size = img.size;

		if (img.format == 1)
			format = GL_LUMINANCE;
		else if (img.format == 2)
			format = GL_LUMINANCE_ALPHA;
		else if (img.format == 3)
			format = GL_RGB;
		else if (img.format == 4)
			format = GL_RGBA;

		glGenTextures(1, &texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, format, texture.size[0], texture.size[1], 0, format, GL_UNSIGNED_BYTE, img.data);

		_activetex = texture.id;
    	_texmap[name] = texture;
    	return (&_texmap[name]);
    }
}

void	Graphicengine::draw_texture(Graphicengine::Texture const *tex, vec<float, 2> const &t, vec<float, 2> const &s, vec<float, 4> const &c)
{
	vec<float, 2>	tc[4] =
	{
		{{0.0f, 0.0f}},
		{{1.0f, 0.0f}},
		{{1.0f, 1.0f}},
		{{0.0f, 1.0f}},
	};

	vec<float, 2>	v[4] =
	{
		{{0.0f, 0.0f}},
		{{1.0f, 0.0f}},
		{{1.0f, 1.0f}},
		{{0.0f, 1.0f}},
	};
	vec<float, 2>	scale;
	scale = vec<float, 2>::cast(tex->size) * s;

	for (unsigned int i = 0; i < 4; ++i)
	{
		v[i] *= scale;
		v[i] += t;
	}

	if (v[2][0] > _top[0] && v[2][1] > _top[1] && v[0][0] < _bot[0] && v[0][1] < _bot[1])
	{
		if (tex->id != _activetex)
		{
			_activetex = tex->id;
			glBindTexture(GL_TEXTURE_2D, tex->id);
		}
		glColor4fv(c.ar);
		glBegin(GL_QUADS);
		for (unsigned int i = 0; i < 4; ++i)
		{
			glTexCoord2fv(tc[i].ar);
			glVertex2fv(v[i].ar);
		}
		glEnd();
	}
}

void		Graphicengine::draw_text(std::string const &text, vec<float, 2> const &trans, vec<float, 2> const &scale, vec<float, 4> const &color)
{
	Text	*t = new Text;
	
	t->text = text;
	t->trans = trans;
	t->scale = scale;
	t->color = color;
	_textlist.push_back(t);
}

inline int	next_p2(int x)
{
    int		a;

	for (a = 1; a < x; a <<= 1);
    return (a);
}

void			Graphicengine::_load_glyph()
{
	FT_Library	library;
	FT_Face		face;
	std::string	fontname = assetsdir + "ascii.ttf";

	if (FT_Init_FreeType(&library))
	{
		std::cerr << "Error: FT_Init_FreeType()" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (FT_New_Face(library, fontname.c_str(), 0, &face))
	{
		std::cerr << "Error: FT_New_Face()" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (FT_Set_Pixel_Sizes(face, 64, 64))
	{
		std::cerr << "Error: FT_Set_Char_Size()" << std::endl;
		exit(EXIT_FAILURE);
	}

	for (unsigned int c = 0; c < 128; ++c)
	{
		if (!FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			FT_Bitmap	*bitmap = &face->glyph->bitmap;

			_glyphtex[c].size[0] = next_p2(bitmap->width);
			_glyphtex[c].size[1] = next_p2(bitmap->rows);
			_glyphtex[c].step[0] = face->glyph->metrics.horiAdvance / 64.0f;
			_glyphtex[c].step[1] = face->glyph->metrics.vertAdvance / 64.0f;
			_glyphtex[c].center[0] = face->glyph->metrics.vertBearingX / 64.0f;
			_glyphtex[c].center[1] = face->glyph->metrics.horiBearingY / 64.0f;

			char *data = new char[_glyphtex[c].size[0] * _glyphtex[c].size[1]];
			for(unsigned int j = 0; j < _glyphtex[c].size[1]; ++j)
			    for(unsigned int i = 0; i < _glyphtex[c].size[0]; ++i)
					data[i + j * _glyphtex[c].size[0]] = (i < (unsigned int)bitmap->width && j < (unsigned int)bitmap->rows) ? bitmap->buffer[i + j * bitmap->width] : 0;

			glGenTextures(1, &_glyphtex[c].id);
			glBindTexture(GL_TEXTURE_2D, _glyphtex[c].id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _glyphtex[c].size[0], _glyphtex[c].size[1], 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);

			delete [] data;
		}
		else
		{
			std::cerr << "Warning: FT_Load_Char() fails on char: " << c << std::endl;
			_glyphtex[c].id = 0;
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void				Graphicengine::_draw_text(Graphicengine::Text const *text)
{
	vec<float, 2>	a = text->trans;
	vec<float, 2>	b;
	unsigned int	c;

	a[1] += 64.0f * text->scale[1];
	for (unsigned int i = 0; i < text->text.size(); ++i)
	{
		b = a;
		c = text->text[i];
		b[1] -= _glyphtex[c].center[1] * text->scale[1];
		draw_texture(&_glyphtex[c], b, text->scale, text->color);
		a[0] += _glyphtex[c].step[0] * text->scale[0];
	}
}
