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

#include <cstdlib>
#include "new.hpp"
#include "configmanager.hpp"
#include "graphicengine.hpp"

Graphicengine::Graphicengine()
	: _renderer(1440, 900, false)
	, _matrices(1024)
	, _rotated_aabbs_infos(4096)
{
	camera.position = 0.0f;
	camera.direction = 0.0f;
	camera.right = 0.0f;
	camera.resolution[0] = 1440;
	camera.resolution[1] = 900;
	camera.fov = 1.745329251994f;

	_animations = new Animation*[_animations_size];

	_lights = new Light[_lights_size];
	_lights_links = new Light**[_lights_size];

	_load_materials();

	_matrices.number = 2;
	for (unsigned int i = 0; i < _matrices.size; ++i)
	{
		_matrices[i] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	}
}

Graphicengine::~Graphicengine()
{
	delete [] _animations;

	delete [] _lights;
	delete [] _lights_links;
}

void	Graphicengine::set_fullscreen(bool const fullscreen)
{
	_renderer.set_fullscreen(fullscreen);
}

void	Graphicengine::set_resolution(unsigned int const width, unsigned int const height)
{
	camera.resolution[0] = width;
	camera.resolution[1] = height;
	_renderer.set_resolution(width, height);
}

void	Graphicengine::set_camera(vec<float, 4> const &position, vec<float, 2> const &spherical_coord)
{
	camera.position = position;

	camera.direction[0] = cos(spherical_coord[1]) * cos(spherical_coord[0]);
	camera.direction[1] = cos(spherical_coord[1]) * sin(spherical_coord[0]);
	camera.direction[2] = sin(spherical_coord[1]);
	camera.direction[3] = 0.0f;

	camera.right[0] = sin(spherical_coord[0]);
	camera.right[1] = -cos(spherical_coord[0]);
	camera.right[2] = 0.0f;
	camera.right[3] = 0.0f;
}

void	Graphicengine::tick(float const delta)
{
	for (unsigned int i = 0; i < _animations_count; ++i)
	{
		if (!_animations[i]->tick(delta))
		{
			delete _animations[i];
			_animations[i--] = _animations[--_animations_count];
		}
	}

	char	str[16];
	_itoa_s((int)(1.0f / delta), str, sizeof(str), 10);
	_renderer.draw_text(str, {0.9f, 0.05f}, {0.0004f, 0.0004f}, {0.9f, 0.5f, 0.125f, 1.0f});

	aabbtree.attach_transient_tree();
	oatree.construct_from(*(AabbTree *)&aabbtree);

	for (unsigned int i = 0; i < oatree._count; ++i)
	{
		if (oatree._nodes[i].children < 0)
		{
			if (oatree._nodes[i].data > 0)
			{
				oatree._nodes[i].children = -oatree._nodes[i].data;
				oatree._nodes[i].data = 0;
			}
			else
			{
				int const infos_index = -oatree._nodes[i].data;
				oatree._nodes[i].aabb = _rotated_aabbs_infos[infos_index].aabb;
				oatree._nodes[i].children = -(int)_rotated_aabbs_infos[infos_index].material_index;
				oatree._nodes[i].data = (int)_rotated_aabbs_infos[infos_index].rotation_index;
			}

		}
	}

	_itoa_s((oatree._count >> 1) + 1, str, sizeof(str), 10);
	_renderer.draw_text(str, {0.9f, 0.1f}, {0.0003f, 0.0003f}, {0.9f, 0.5f, 0.125f, 1.0f});

	_renderer.render(this);
	aabbtree.delete_transient_tree();

	_rotated_aabbs_infos.clear();
}

void	Graphicengine::add_animation(Animation *animation)
{
	if (_animations_count >= _animations_size)
	{
		_animations_size <<= 1;
		_animations = resize(_animations, _animations_count, _animations_size);
	}
	_animations[_animations_count++] = animation;
}

void	Graphicengine::remove_animation(Animation *animation)
{
	int	index;
	for (index = _animations_count - 1; index >= 0 && _animations[index] != animation; --index)
		;
	if (index >= 0)
		_animations[index] = _animations[--_animations_count];
}

int		Graphicengine::add_static_block(Aabb const & aabb, unsigned int const material)
{

}

void	Graphicengine::remove_static_block(unsigned int const)
{

}

unsigned int	Graphicengine::add_rotation(Quatermion<float> const & quatermion)
{

}

void			Graphicengine::add_dynamic_block(Aabb const & aabb, unsigned int const rotation, unsigned int const material)
{
	vec<float, 4> const &	center = (aabb.bottom + aabb.top) * 0.5f;
	Aabb const &			centered_aabb = { aabb.bottom - center, aabb.top - center };

	vec<vec<float, 4>, 8>	rotated_points =
	{
		centered_aabb.bottom[0]		, centered_aabb.bottom[1]	, centered_aabb.bottom[2]	, 0.0f ,
		centered_aabb.bottom[0]		, centered_aabb.bottom[1]	, centered_aabb.top[2]		, 0.0f ,
		centered_aabb.bottom[0]		, centered_aabb.top[1]		, centered_aabb.bottom[2]	, 0.0f ,
		centered_aabb.bottom[0]		, centered_aabb.top[1]		, centered_aabb.top[2]		, 0.0f ,
		centered_aabb.top[0]		, centered_aabb.bottom[1]	, centered_aabb.bottom[2]	, 0.0f ,
		centered_aabb.top[0]		, centered_aabb.bottom[1]	, centered_aabb.top[2]		, 0.0f ,
		centered_aabb.top[0]		, centered_aabb.top[1]		, centered_aabb.bottom[2]	, 0.0f ,
		centered_aabb.top[0]		, centered_aabb.top[1]		, centered_aabb.top[2]		, 0.0f
	};

	rotated_points[0] = _matrices[rotation] * rotated_points[0];
	rotated_points[1] = _matrices[rotation] * rotated_points[1];
	rotated_points[2] = _matrices[rotation] * rotated_points[2];
	rotated_points[3] = _matrices[rotation] * rotated_points[3];
	rotated_points[4] = _matrices[rotation] * rotated_points[4];
	rotated_points[5] = _matrices[rotation] * rotated_points[5];
	rotated_points[6] = _matrices[rotation] * rotated_points[6];
	rotated_points[7] = _matrices[rotation] * rotated_points[7];

	Aabb	rotated_aabb;

	rotated_aabb.bottom = 	vmin(rotated_points[0],
							vmin(rotated_points[1],
							vmin(rotated_points[2],
							vmin(rotated_points[3],
							vmin(rotated_points[4],
							vmin(rotated_points[5],
							vmin(rotated_points[6],
							rotated_points[7])))))));

	rotated_aabb.top = 		vmax(rotated_points[0],
							vmax(rotated_points[1],
							vmax(rotated_points[2],
							vmax(rotated_points[3],
							vmax(rotated_points[4],
							vmax(rotated_points[5],
							vmax(rotated_points[6],
							rotated_points[7])))))));

	rotated_aabb.bottom += center;
	rotated_aabb.top += center;

	int const	infos_index = -(int)_rotated_aabbs_infos.number;
	_rotated_aabbs_infos.allocate() = { aabb, rotation, material };

	aabbtree.add_transient_aabb(rotated_aabb, infos_index);
}

void	Graphicengine::new_light(Light **link)
{
	if (_lights_count >= _lights_size)
	{
		_lights_size <<= 1;
		_lights = resize(_lights, _lights_count, _lights_size);
		_lights_links = resize(_lights_links, _lights_count, _lights_size);

		for (unsigned int i = 0; i < _lights_count; ++i)
			*_lights_links[i] = _lights + i;
	}

	_lights_links[_lights_count] = link;
	*link = _lights + _lights_count++;
}

void	Graphicengine::delete_light(Light *light)
{
	int	index;

	index = (int)(light - _lights);
	_lights[index] = _lights[--_lights_count];
	_lights_links[index] = _lights_links[_lights_count];
	*_lights_links[index] = _lights + index;
}

void				Graphicengine::_load_materials()
{
	DFNode const	*nd = Configmanager::get_instance().get("materials");

	unsigned int const	size = nd->size;
	_materials.reset(size, size, (Material const * const)nd->flt);
}
