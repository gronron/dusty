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

#include <CL/cl.h>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include "raytracer.hpp"

#include "opencl_error.hpp"
#include "new.hpp"
#include "math/vec_util.hpp"

#include <io.h>
#include <fcntl.h>

static char		*read_file(char const *filename)
{
	int			id;
	int			size;
	char		*buffer;

	buffer = 0;
	_sopen_s(&id, filename, _O_RDONLY, _SH_DENYNO, _S_IREAD);
	if (id >= 0)
	{
		buffer = new char[65536];
		size = _read(id, buffer, 65536);
		if (size >= 0)
			buffer[size] = '\0';
		else
		{
			delete [] buffer;
			buffer = 0;
		}
		_close(id);
	}
	else
		perror("read_file(): ");
	return (buffer);
}

Raytracer::Raytracer(unsigned int const width, unsigned int const height) : materials_size(256),
																			materials_count(0),
																			materials(0),
																			_lights_size(4096),
																			_lights_count(0),
																			_lights(0),
																			_lights_links(0),
																			_nodes_mem_size(0),
																			_materials_mem_size(0),
																			_lights_mem_size(0),
																			image(0)
{
	cl_int			error;
	cl_uint			platforms_count;
	cl_platform_id	*platforms;
	cl_uint 		devices_count;
	cl_device_id	*devices;
	
	camera.spherical_coord[0] = -2.2f;
	camera.spherical_coord[1] = -0.7f;
	camera.fov = 1.5f;
	
	materials = new Material[materials_size];
	
	_lights = new Light[_lights_size];
	_lights_links = new Light**[_lights_size];
	
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "Error: SDL_Init()" << std::endl;
		exit(EXIT_FAILURE);
	}
	window = SDL_CreateWindow("dusty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, /*SDL_WINDOW_OPENGL | */SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	//glcontext = SDL_GL_CreateContext(window);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	check_error(clGetPlatformIDs(0, 0, &platforms_count), "clGetPlatformIDs()");
	platforms = new cl_platform_id[platforms_count];
	check_error(clGetPlatformIDs(platforms_count, platforms, 0), "clGetPlatformIDs()");

	cl_context_properties const	properties[] =
	{
		CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0],
		//CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
		//CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		0
	};
	context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, 0, 0, &error);
	check_error(error, "clCreateContextFromType()");
	delete [] platforms;

	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, 0, &devices_count);
	devices = new cl_device_id[devices_count];
	clGetContextInfo(context, CL_CONTEXT_DEVICES, devices_count, devices, 0);

	queue = clCreateCommandQueueWithProperties(context, devices[0], 0, &error);
	check_error(error, "clCreateCommandQueueWithProperties()");

	char const	*source = read_file("raytracer.cl");
		
	if (!source)
		exit(EXIT_FAILURE);
	size_t const	source_length[] = { strlen(source) };
	program = clCreateProgramWithSource(context, 1, &source, source_length, &error);
	check_error(error, "clCreateProgramWithSource()");
	
	error = clBuildProgram(program, 1, &devices[0], "-w", 0, 0);
	if (error != CL_SUCCESS)
	{
		unsigned int	length;
		char			*build_log;

		std::cerr << "clBuildProgram(): " << error_to_string(error) << std::endl;
		check_error(clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, 0, &length), "");
		build_log = new char[length + 1];
		check_error(clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, length, build_log, 0), "");
		build_log[length] = '\0';
		std::cerr << build_log << std::endl;
		system("pause");
		delete [] build_log;
		exit(EXIT_FAILURE);
	}
	delete [] source;
	delete [] devices;

	kernel = clCreateKernel(program, "raytrace", &error);
	check_error(error, "clCreateKernel()");

	set_resolution(width, height);
	
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

Raytracer::~Raytracer()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
	
	delete [] materials;
	
	delete [] _lights;
	delete [] _lights_links;
	
	clReleaseMemObject(nodes_mem);
	clReleaseMemObject(materials_mem);
	clReleaseMemObject(lights_mem);
	clReleaseMemObject(image_mem);
	
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void		Raytracer::_set_buffer()
{
	cl_int	error;

	if (_nodes_mem_size < aabbtree._size)
	{
		if (_nodes_mem_size)
			clReleaseMemObject(nodes_mem);
		_nodes_mem_size = aabbtree._size;
		nodes_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _nodes_mem_size * sizeof(Node), 0, &error);
		check_error(error, "clCreateBuffer()");
	}
	if (_materials_mem_size < materials_size)
	{
		if (_materials_mem_size)
			clReleaseMemObject(materials_mem);
		_materials_mem_size = materials_size;
		materials_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _materials_mem_size * sizeof(Material), 0, &error);
		check_error(error, "clCreateBuffer()");
	}
	if (_lights_mem_size < _lights_size)
	{
		if (_lights_mem_size)
			clReleaseMemObject(lights_mem);
		_lights_mem_size = _lights_size;
		lights_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _lights_mem_size * sizeof(Light), 0, &error);
		check_error(error, "clCreateBuffer()");
	}
	check_error(clEnqueueWriteBuffer(queue, nodes_mem, CL_TRUE, 0, aabbtree._size * sizeof(Node), (void *)aabbtree._nodes, 0, 0, 0), "clEnqueueWriteBuffer()");
	check_error(clEnqueueWriteBuffer(queue, materials_mem, CL_TRUE, 0, materials_count * sizeof(Material), (void *)materials, 0, 0, 0), "clEnqueueWriteBuffer()");
	check_error(clEnqueueWriteBuffer(queue, lights_mem, CL_TRUE, 0, _lights_count * sizeof(Light), (void *)_lights, 0, 0, 0), "clEnqueueWriteBuffer()");
}

void	Raytracer::set_resolution(unsigned int const width, unsigned int const height)
{
	if (image)
	{
		SDL_FreeSurface(image);
		clReleaseMemObject(image_mem);
	}
	
	camera.resolution[0] = width;
	camera.resolution[1] = height;
	
	image = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

	cl_int					error;
	cl_image_format const	format = { CL_RGBA, CL_UNSIGNED_INT8 };
	cl_image_desc const		desc = { CL_MEM_OBJECT_IMAGE2D, width, height, 1, 1, 0, 0, 0 };
	
	image_mem = clCreateImage(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, &format, &desc, 0, &error);
	check_error(error, "clCreateImage()");
}

void	Raytracer::tick(float const delta)
{
	_render();
}

void	Raytracer::_render()
{
	Computedcamera	cm;

	_compute_camera(cm);
	_set_buffer();
	
	unsigned int	argc = 0;
	check_error(clSetKernelArg(kernel, argc++, sizeof(Computedcamera), &cm), "clSetKernelArg()0");
	check_error(clSetKernelArg(kernel, argc++, sizeof(int), &aabbtree._root), "clSetKernelArg()1");
	check_error(clSetKernelArg(kernel, argc++, sizeof(cl_mem), &nodes_mem), "clSetKernelArg()2");
	check_error(clSetKernelArg(kernel, argc++, sizeof(unsigned int), &materials_count), "clSetKernelArg()3");
	check_error(clSetKernelArg(kernel, argc++, sizeof(cl_mem), &materials_mem), "clSetKernelArg()4");
	check_error(clSetKernelArg(kernel, argc++, sizeof(unsigned int), &_lights_count), "clSetKernelArg()5");
	check_error(clSetKernelArg(kernel, argc++, sizeof(cl_mem), &lights_mem), "clSetKernelArg()6");
	check_error(clSetKernelArg(kernel, argc++, sizeof(cl_mem), &image_mem), "clSetKernelArg()7");

	unsigned int	local_work_size = 64;
	check_error(clEnqueueNDRangeKernel(queue, kernel, 2, 0, (size_t *)&camera.resolution, 0, 0, 0, 0), "clEnqueueNDRangeKernel()");

	size_t const	origin[3] = { 0, 0, 0 };
	size_t const	region[3] = { camera.resolution[0], camera.resolution[1], 1 };
	check_error(clEnqueueReadImage(queue, image_mem, CL_TRUE, origin, region, 0, 0, image->pixels, 0, 0, 0), "clEnqueueReadImage()");

	SDL_Texture	*texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, 0, 0);
	SDL_DestroyTexture(texture);
	SDL_RenderPresent(renderer);
}

void	Raytracer::_compute_camera(Computedcamera &cm)
{
	vec<float, 4>	direction;
	
	int	x;
	int	y;
	SDL_GetRelativeMouseState(&x, &y);
	camera.spherical_coord[0] -= x / 200.0f;
	camera.spherical_coord[1] -= y / 200.0f;

	direction[0] = cos(camera.spherical_coord[1]) * cos(camera.spherical_coord[0]);
	direction[1] = cos(camera.spherical_coord[1]) * sin(camera.spherical_coord[0]);
	direction[2] = sin(camera.spherical_coord[1]);
	direction[3] = 0.0f;
	
	//camera.position += direction * 0.1f;

	cm.position = camera.position;
	cm.forward = (direction * (float)camera.resolution[0]) / (tan(camera.fov / 2.0f) * 2.0f);
	cm.right[0] = sin(camera.spherical_coord[0]);
	cm.right[1] = -cos(camera.spherical_coord[0]);
	cm.right[2] = 0.0f;
	cm.right[3] = 0.0f;
	cm.right = vunit<float>(cm.right);
	cm.up = vunit<float>(vcross(direction, cm.right));
	
	cm.half_resolution = (vec<float, 2>)camera.resolution / 2.0f;
}

///////////////////////////////////////

void	Raytracer::new_light(Light **link)
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

void	Raytracer::delete_light(Light *light)
{
	int	index;

	index = light - _lights;
	_lights[index] = _lights[--_lights_count];
	_lights_links[index] = _lights_links[_lights_count];
}
