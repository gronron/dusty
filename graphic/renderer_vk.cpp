/******************************************************************************
Copyright (c) 2015-2017, Geoffrey TOURON
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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"0
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

#define GLEW_STATIC
#include <glew.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <iostream>
#include <cstdlib>
#include "new.hpp"
#include "math/vec_util.hpp"
#include "file/file.hpp"
#include "graphicengine.hpp"
#include "renderer_vk.hpp"

struct				Computedcamera
{
	vec<float, 4>	position;
	vec<float, 4>	forward;
	vec<float, 4>	right;
	vec<float, 4>	up;
	vec<float, 2>	half_resolution;
};

void	_compute_camera(Camera const &camera, Computedcamera &cm)
{
	cm.position = camera.position;
	cm.forward = (camera.direction * (float)camera.resolution[0]) / (tan(camera.fov / 2.0f) * 2.0f);
	cm.right = camera.right;
	cm.up = vunit<float>(vcross(camera.direction, camera.right));
	cm.half_resolution = (vec<float, 2>)camera.resolution / 2.0f;
}

unsigned int	_load_font(char const *, Glyph *);

Renderer::Renderer(unsigned int const w, unsigned int const h, bool const fullscreen) : width(w), height(h), _window(0), _glcontext(0), _texture(0), _nodes_mem_size(0), _materials_mem_size(0), _lights_mem_size(0)
{
	SDL_SysWMinfo 	info;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "error! SDL_Init()" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	_window = SDL_CreateWindow("dusty", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
	_glcontext = SDL_GL_CreateContext(_window);
	SDL_GL_SetSwapInterval(0);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo(_window, &info))
	{
		std::cerr << "error! SDL_GetWindowWMInfo()" << std::endl;
		exit(EXIT_FAILURE);
	}

}

Renderer::~Renderer()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);

	SDL_GL_DeleteContext(_glcontext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void	Renderer::set_fullscreen(bool const fullscreen)
{
	SDL_SetWindowFullscreen(_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void	Renderer::set_resolution(unsigned int const w, unsigned int const h)
{
	width = w;
	height = h;
}

void		Renderer::_set_buffer(Graphicengine const *ge)
{

}

void	Renderer::render(Graphicengine const *ge)
{
	Computedcamera	cm;

	_compute_camera(ge->camera, cm);

}
