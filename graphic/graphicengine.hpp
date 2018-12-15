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
#include "math/matrix.hpp"
#include "math/quatermion.hpp"
#include "haabbtree.hpp"
#include "renderer_gl.hpp"
#include "animation.hpp"

struct						Camera
{
	vec<float, 4>			position;
	vec<float, 4>			direction;
	vec<float, 4>			right;
	vec<unsigned int, 2>	resolution;
	float					fov;
};

struct				Light
{
	vec<float, 4>	position;
	vec<float, 3>	color;
	float			power;
};

struct				Material
{
	vec<float, 3>	color;
	float			transparency;
	float			refraction;
	float			reflection;
	float			shining;
	float			emissivity;
};

struct				RotatedAabbInfos
{
	Aabb			aabb;
	unsigned int	rotation_index;
	unsigned int	material_index;
};

using mat3 = Matrix<float, 4, 3>;

class	Graphicengine
{
public:

	Camera			camera;

	unsigned int	_animations_size = 4096;
	unsigned int	_animations_count = 0;
	Animation		**_animations = nullptr;

	unsigned int	_lights_size= 64;
	unsigned int	_lights_count = 0;
	Light			*_lights = nullptr;
	Light			***_lights_links = nullptr;


	DynamicArray<mat3>				_matrices;
	DynamicArray<Material>			_materials;
	DynamicArray<RotatedAabbInfos>	_rotated_aabbs_infos;

	Haabbtree		aabbtree;
	OrderedAabbTree	oatree;

	Renderer		_renderer;


	Graphicengine();
	~Graphicengine();

	void	set_fullscreen(bool const);
	void	set_resolution(unsigned int const, unsigned int const);
	void	set_camera(vec<float, 4> const &, vec<float, 2> const &);

	void	tick(float const delta);

	void	add_animation(Animation *);
	void	remove_animation(Animation *);

	int		add_static_block(Aabb const & aabb, unsigned int const material);
	void	remove_static_block(unsigned int const);

	unsigned int	add_rotation(Quatermion<float> const & quatermion);
	void			add_dynamic_block(Aabb const & aabb, unsigned int const rotation, unsigned int const material);

	void	new_light(Light **);
	void	delete_light(Light *);

	void			draw_text(char const *, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &);
	void			draw_text(unsigned int const, char const *, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &);
	unsigned int	cut_line(char const *, vec<float, 2> const &, float const) const;

	void	_load_materials(char const *filename);
};

inline void	Graphicengine::draw_text(char const *text, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color)
{
	_renderer.draw_text(text, position, scale, color);
}

inline void	Graphicengine::draw_text(unsigned int const size, char const *text, vec<float, 2> const &position, vec<float, 2> const &scale, vec<float, 4> const &color)
{
	_renderer.draw_text(size, text, position, scale, color);
}

inline unsigned int	Graphicengine::cut_line(char const *text, vec<float, 2> const &scale, float const width) const
{
	return (_renderer.cut_line(text, scale, width));
}
