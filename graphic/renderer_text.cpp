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

#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "renderer.hpp"

inline int	next_p2(int const x)
{
    int		a;

	for (a = 1; a < x; a <<= 1);
    return (a);
}

unsigned int	_load_font(char const *filename, Glyph *glyphs)
{
	FT_Library	library;
	FT_Face		face;

	if (FT_Init_FreeType(&library))
	{
		std::cerr << "error! FT_Init_FreeType()" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (FT_New_Face(library, filename, 0, &face))
	{
		std::cerr << "error! FT_New_Face() fails to open: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	if (FT_Set_Pixel_Sizes(face, 64, 64))
	{
		std::cerr << "error! FT_Set_Char_Size()" << std::endl;
		exit(EXIT_FAILURE);
	}

	unsigned int			texture;
	vec<unsigned int, 2>	size = { 0, 0 };
	unsigned char			*data;
	unsigned int			cursor = 0;

	for (unsigned int c = 0; c < 128; ++c)
	{
		if (!FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			FT_Bitmap	*bitmap = &face->glyph->bitmap;

			size[0] += bitmap->width + 1;
			if (bitmap->rows > size[1])
				size[1] = bitmap->rows;
		}
		else
			std::cerr << "warning! FT_Load_Char() fails on char: " << c << std::endl;
	}

	size[0] = next_p2(size[0] - 1);
	data = new unsigned char[size[0] * size[1]];

	for (unsigned int i = 0; i < size[0] * size[1]; ++i)
		data[i] = 0;

	for (unsigned int c = 0; c < 128; ++c)
	{
		if (!FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			FT_Bitmap const	*bitmap = &face->glyph->bitmap;

			unsigned int const offset = cursor;
			glyphs[c].topleft[0] = (float)cursor / size[0];
			glyphs[c].topleft[1] = 0;
			cursor += bitmap->width;
			glyphs[c].bottomright[0] = (float)cursor++ / size[0];
			glyphs[c].bottomright[1] = (float)bitmap->rows / size[1];
			glyphs[c].size[0] = bitmap->width;
			glyphs[c].size[1] = bitmap->rows;
			glyphs[c].step[0] = face->glyph->metrics.horiAdvance / 64.0f;
			glyphs[c].step[1] = face->glyph->metrics.vertAdvance / 64.0f;
			glyphs[c].center[0] = face->glyph->metrics.vertBearingX / 64.0f;
			glyphs[c].center[1] = face->glyph->metrics.horiBearingY / 64.0f;

			for(unsigned int j = 0; j < bitmap->rows; ++j)
			    for(unsigned int i = 0; i < bitmap->width; ++i)
					data[j * size[0] + offset + i] = bitmap->buffer[j * bitmap->width + i];
		}
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, size[0], size[1], 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);

	delete [] data;

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return (texture);
}

void	Renderer::draw_text(char const *text, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color) const
{
	vec<float, 2>	a = position;
	vec<float, 2>	b;
	
	a[1] += 64.0f * scale[1];

	glBindTexture(GL_TEXTURE_2D, _glyphstexture);
	glColor4fv(color.ar);

	for (unsigned int i = 0; text[i]; ++i)
	{
		unsigned char const	c = text[i];

		b = a;
		b[1] -= _glyphs[c].center[1] * scale[1];
		a[0] += _glyphs[c].step[0] * scale[0];
		
		vec<float, 2> const	tc[4] =
		{
			{ { _glyphs[c].topleft[0], _glyphs[c].topleft[1] } },
			{ { _glyphs[c].bottomright[0], _glyphs[c].topleft[1] } },
			{ { _glyphs[c].bottomright[0], _glyphs[c].bottomright[1] } },
			{ { _glyphs[c].topleft[0], _glyphs[c].bottomright[1] } },
		};

		vec<float, 2>	v[4] =
		{
			{{0.0f, 0.0f}},
			{{1.0f, 0.0f}},
			{{1.0f, 1.0f}},
			{{0.0f, 1.0f}},
		};
		vec<float, 2> const	s = _glyphs[c].size * scale;

		for (unsigned int i = 0; i < 4; ++i)
			v[i] = v[i] * s + b;

		if (v[2][0] > 0.0f && v[2][1] > 0.0f && v[0][0] < width && v[0][1] < height)
		{
			glBegin(GL_QUADS);
			for (unsigned int i = 0; i < 4; ++i)
			{
				glTexCoord2fv(tc[i].ar);
				glVertex2fv(v[i].ar);
			}
			glEnd();
		}
	}
}
