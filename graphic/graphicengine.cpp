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
#include <cstdlib>
#include "configmanager.hpp"
#include "image.hpp"
#include "graphicengine.hpp"

#include <GL/glew.h>
#include <SDL.h>
#include <freetype/ft2build.h>
#include FT_FREETYPE_H


Graphicengine::Graphicengine() : assetsdir(), screensize(), _window(), _glcontext(), _activetex(0)
{
	GLenum	error;
	Df_node	*config;
	
	assetsdir = Configmanager::get_instance().assetsdir;
	assetsdir = "assets\\";
	config = Configmanager::get_instance().get("graphic_cfg.df");
	if (!config->get("screensize", Df_node::INT, 2, &screensize))
	{
		screensize[0] = 1024;
		screensize[1] = 768;
	}
	screensize[0] = 1024;
	screensize[1] = 768;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Error: SDL_Init() " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	_window = SDL_CreateWindow("dusty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screensize[0], screensize[1], SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	_glcontext = SDL_GL_CreateContext(_window);

	glewExperimental = GL_TRUE;
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
	SDL_GL_DeleteContext(_glcontext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
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
	_bot = -_top + cam_pos;
	_top += cam_pos;

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
	
	glFlush();
	SDL_GL_SwapWindow(_window);
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
