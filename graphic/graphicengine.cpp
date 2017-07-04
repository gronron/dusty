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

#include <cstdlib>
#include "new.hpp"
#include "configmanager.hpp"
#include "graphicengine.hpp"

Graphicengine::Graphicengine() :	_animations_size(4096), _animations_count(0), _animations(0),
									_lights_size(4096), _lights_count(0), _lights(0), _lights_links(0),
									_materials_size(0), _materials_count(0), _materials(0),
									_renderer(1440, 900, false)
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

	_load_materials("materials.df");
}

Graphicengine::~Graphicengine()
{
	delete [] _animations;
	
	delete [] _lights;
	delete [] _lights_links;
	
	delete [] _materials;
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
	_renderer.draw_text(str, {camera.resolution[0] - 64.0f, 16.0f}, {0.3f, 0.3f}, {0.35f, 0.6f, 0.35f, 1.0f});

	aabbtree.attach_transient_tree();
	_renderer.render(this);
	aabbtree.delete_transient_tree();
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

void				Graphicengine::_load_materials(char const *filename)
{
	Df_node const	*nd = Configmanager::get_instance().get(filename);
	
	_materials_count = nd->data_size / sizeof(Material);
	_materials_size = nd->data_size / sizeof(Material);
	_materials = new Material[256];
	memcpy(_materials, nd->data_storage, nd->data_size);
}
