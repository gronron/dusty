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

#include "dynamic_array.hpp"
#include "math/vec.hpp"

#include <unordered_map>

class	Graphicengine;

struct	Glyph final
{
	vec<float, 2>	size;
	vec<float, 2>	topleft;
	vec<float, 2>	bottomright;
	vec<float, 2>	step;
	vec<float, 2>	center;
};

struct	Vertex final
{
	vec<float, 4>	position;
	vec<float, 4>	color;
	vec<float, 2>	coord;
	float			padding[2];
};

struct	TextureLocation final
{
	vec<float, 2>	topleft;
	vec<float, 2>	bottomright;
	unsigned int	texture_id;
};

class	RTRenderer final
{
public:

	unsigned int	_program = 0;
	unsigned int	_vao = 0;
	unsigned int 	_vbo = 0;

	int				_camera_idx = 0;
	int				_nodes_idx = 0;
	int				_matrices_idx = 0;
	int				_materials_idx = 0;
	int				_lights_nbr_idx = 0;
	int				_lights_idx = 0;

	unsigned int	_camera_buffer = 0;
	unsigned int	_nodes_buffer = 0;
	unsigned int	_matrices_buffer = 0;
	unsigned int	_materials_buffer = 0;
	unsigned int	_lights_buffer = 0;

	unsigned int	_nodes_mem_size = 0;
	unsigned int	_matrices_mem_size = 0;
	unsigned int	_materials_mem_size = 0;
	unsigned int	_lights_mem_size = 0;


	RTRenderer();
	~RTRenderer();

	RTRenderer(RTRenderer const &) = delete;
	RTRenderer const &	operator=(RTRenderer const &) = delete;


	void	_init();
	void	_set_buffer(Graphicengine const *ge);
	void	_render();
};

class	FlatRenderer final
{
public:

	unsigned int	_program = 0;
	unsigned int 	_vao = 0;
	unsigned int 	_vbo = 0;

	unsigned int	_mem_size = 0;

	DynamicArray<unsigned int>								_texture_ids;
	std::unordered_map<unsigned int, TextureLocation>		_texture_locations; //key = absolute texture id by hash
	std::unordered_map<unsigned int, DynamicArray<Vertex>>	_vertices; //key = opengl texture_id

	FlatRenderer();
	~FlatRenderer();

	FlatRenderer(FlatRenderer const &) = delete;
	FlatRenderer const &	operator=(FlatRenderer const &) = delete;

	void	draw_texture(unsigned int const texture_id, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color);

	void	_init();
	void	_set_buffer(Graphicengine const *ge);
	void	_render();
};

class	TextRenderer final
{
public:
	unsigned int	_program = 0;
	unsigned int 	_vao = 0;
	unsigned int 	_vbo = 0;

	unsigned int	_mem_size = 0;

	unsigned int	_glyphstexture = 0;
	Glyph			_glyphs[128];

	DynamicArray<Vertex>	_vertices;


	TextRenderer();
	~TextRenderer();

	TextRenderer(TextRenderer const &) = delete;
	TextRenderer const &	operator=(TextRenderer const &) = delete;

	void			draw_text(char const *text, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color);
	void			draw_text(unsigned int const size, char const *text, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color);
	unsigned int	cut_line(char const *text, vec<float, 2> const &scale, float const width) const;

	void	_init();
	void	_set_buffer(Graphicengine const *ge);
	void	_render();
};


class	Renderer final
{
public:

	unsigned int	width = 0;
	unsigned int	height = 0;

	void			*_window = nullptr;
	void			*_glcontext = nullptr;

	RTRenderer		_rt_renderer;
	FlatRenderer	_flat_renderer;
	TextRenderer	_text_renderer;


	Renderer(unsigned int const, unsigned int const, bool const);
	~Renderer();

	Renderer(Renderer const &) = delete;
	Renderer const &	operator=(Renderer const &) = delete;

	void			set_fullscreen(bool const);
	void			set_resolution(unsigned int const, unsigned int const);

	void			draw_text(char const *text, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color) { _text_renderer.draw_text(text, position, scale, color); }
	void			draw_text(unsigned int const size, char const *text, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color) { _text_renderer.draw_text(size, text, position, scale, color); }
	unsigned int	cut_line(char const *text, vec<float, 2> const &scale, float const width) const { return (_text_renderer.cut_line(text, scale, width)); }

	void			render(Graphicengine const *);
};

void			_check_error(int const line);
unsigned int	_build_program(char const * const vertex_name, char const * const frag_name);
