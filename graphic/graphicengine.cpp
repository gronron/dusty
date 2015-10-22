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

#include "graphicengine.hpp"

Graphicengine::Graphicengine()
{
	_animation = new *Animation[_animations_size];

	_lights = new Light[_lights_size];
	_lights_links = new Light**[_lights_size];
}

Graphicengine::~Graphicengine()
{
	delete [] _animation;
	
	delete [] _lights;
	delete [] _lights_links;
}

Graphicengine::set_resolution()
{
	
}

void	Graphicengine::set_resolution(unsigned int const, unsigned int const)
{

}

void	Graphicengine::tick(float const delta)
{
	for (unsigned int i = 0; i < _animations_count; ++i)
		_animations[i]->tick(delta);

	_renderer.render();
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
	for (index = _animations_count - 1; index >= && _animations[index] != animation; --index)
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

	index = light - _lights;
	_lights[index] = _lights[--_lights_count];
	_lights_links[index] = _lights_links[_lights_count];
}

bool	Graphicengine::load_materials(char const *filename)
{
	
}
