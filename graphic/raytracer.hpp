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

#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include <CL/cl.h>
//#include <CL/cl_gl.h>
#include <SDL.h>
#include "aabbtree.hpp"

struct						Camera
{
	vec<float, 4>			position;
	vec<float, 2>			spherical_coord;
	vec<unsigned int, 2>	resolution;
	float					fov;
};

struct				Computedcamera
{
	vec<float, 4>	position;
	vec<float, 4>	forward;
	vec<float, 4>	right;
	vec<float, 4>	up;
	vec<float, 2>	half_resolution;
	float			padding[2];
};

struct				Material
{
	vec<float, 3>	color;
	float			transparency;
	float			reflection;
	float			shining;
	float			padding[2];
};

struct				Light
{
	vec<float, 4>	position;
	vec<float, 3>	color;
	float			power;
};

class	Raytracer
{
	public:
	
		Camera			camera;

		unsigned int	materials_size;
		unsigned int	materials_count;
		Material		*materials;

		unsigned int	_lights_size;
		unsigned int	_lights_count;
		Light			*_lights;
		Light			***_lights_links;

		Aabbtree		aabbtree;
		
		//sdl
		
		SDL_Window		*window;
		//SDL_GLContext	glcontext = SDL_GL_CreateContext(window);
		SDL_Renderer	*renderer;
		SDL_Surface		*image;
		
		//opencl

		cl_context			context;
		cl_command_queue	queue;
		cl_program			program;
		cl_kernel			kernel;
		
		unsigned int		_nodes_mem_size;
		unsigned int		_materials_mem_size;
		unsigned int		_lights_mem_size;
		
		cl_mem		nodes_mem;
		cl_mem		materials_mem;
		cl_mem		lights_mem;
		cl_mem		image_mem;


		Raytracer(unsigned int const, unsigned int const);
		~Raytracer();

		
		void	set_resolution(unsigned int const, unsigned int const);
		
		void	tick(float const);

		void	_render();
		void	_set_buffer();
		void	_compute_camera(Computedcamera &);
		
		void	new_light(Light **);
		void	delete_light(Light *);
};

#endif
