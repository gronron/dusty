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

#pragma once

#include "array.hpp"
#include "math/vec.hpp"

class	Graphicengine;

struct	Glyph
{
	vec<float, 2>	size;
	vec<float, 2>	topleft;
	vec<float, 2>	bottomright;
	vec<float, 2>	step;
	vec<float, 2>	center;
};

struct	CharVertex
{
	vec<float, 4>	position;
	vec<float, 4>	color;
	vec<float, 2>	coord;
	float			padding[2];
};

class	Renderer
{
public:

	unsigned int	width;
	unsigned int	height;

	void			*_window;
	void			*_glcontext;

	unsigned int	_main_program;
	unsigned int	_text_program;

	unsigned int	_main_vao;
	unsigned int 	_main_vbo;

	unsigned int 	_text_vao;
	unsigned int 	_text_vbo;

	int				_cameraidx;
	int				_nodesidx;
	int				_materialsidx;
	int				_lightsnbridx;
	int				_lightsidx;

	unsigned int	_camerabuffer;
	unsigned int	_nodesbuffer;
	unsigned int	_materialsbuffer;
	unsigned int	_lightsbuffer;

	unsigned int	_text_mem_size;
	unsigned int	_nodes_mem_size;
	unsigned int	_materials_mem_size;
	unsigned int	_lights_mem_size;

	unsigned int	_glyphstexture;
	Glyph			_glyphs[128];

	Array<CharVertex>	_text_vertices;


	Renderer(unsigned int const, unsigned int const, bool const);
	~Renderer();

	void			set_fullscreen(bool const);
	void			set_resolution(unsigned int const, unsigned int const);
	void			draw_text(char const *, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &);
	void			draw_text(unsigned int const, char const *, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &);
	unsigned int	cut_line(char const *, vec<float, 2> const &, float const) const;

	void			render(Graphicengine const *);

	void			_init_main_renderer();
	void			_init_text_renderer();

	void			_set_buffer(Graphicengine const *);
};
