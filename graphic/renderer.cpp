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
#include <SDL.h>
#include <iostream>
#include <cstdlib>
#include "opencl_error.hpp"
#include "new.hpp"
#include "math/vec_util.hpp"
#include "graphicengine.hpp"
#include "renderer.hpp"

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

struct				Computedcamera
{
	vec<float, 4>	position;
	vec<float, 4>	forward;
	vec<float, 4>	right;
	vec<float, 4>	up;
	vec<float, 2>	half_resolution;
	float			padding[2];
};

void				_compute_camera(Camera const &camera, Computedcamera &cm)
{
	vec<float, 4>	direction;

	direction[0] = cos(camera.spherical_coord[1]) * cos(camera.spherical_coord[0]);
	direction[1] = cos(camera.spherical_coord[1]) * sin(camera.spherical_coord[0]);
	direction[2] = sin(camera.spherical_coord[1]);
	direction[3] = 0.0f;

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

Renderer::Renderer(unsigned int const width, unsigned int const height) :	_nodes_mem_size(0),
																			_materials_mem_size(0),
																			_lights_mem_size(0),
																			texture(0)
{
	cl_int			error;
	cl_uint			platforms_count;
	cl_platform_id	*platforms;
	size_t			devices_count;
	cl_device_id	*devices;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "Error: SDL_Init()" << std::endl;
		exit(EXIT_FAILURE);
	}
	window = SDL_CreateWindow("dusty", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE/* | SDL_WINDOW_FULLSCREEN_DESKTOP*/);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	check_error(clGetPlatformIDs(0, 0, &platforms_count), "clGetPlatformIDs()");
	platforms = new cl_platform_id[platforms_count];
	check_error(clGetPlatformIDs(platforms_count, platforms, 0), "clGetPlatformIDs()");

	cl_context_properties const	properties[] =
	{
		CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0], 0
	};
	_context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, 0, 0, &error);
	check_error(error, "clCreateContextFromType()");
	delete [] platforms;

	clGetContextInfo(_context, CL_CONTEXT_DEVICES, 0, 0, &devices_count);
	devices = new cl_device_id[devices_count];
	clGetContextInfo(_context, CL_CONTEXT_DEVICES, devices_count, devices, 0);

	_queue = clCreateCommandQueueWithProperties(_context, devices[0], 0, &error);
	check_error(error, "clCreateCommandQueueWithProperties()");

	char const	*source = read_file("raytracer.cl");

	if (!source)
		exit(EXIT_FAILURE);
	size_t const	source_length[] = { strlen(source) };
	_program = clCreateProgramWithSource(_context, 1, &source, source_length, &error);
	check_error(error, "clCreateProgramWithSource()");

	error = clBuildProgram(_program, 1, &devices[0], "-w", 0, 0);
	if (error != CL_SUCCESS)
	{
		size_t			length;
		char			*build_log;

		std::cerr << "clBuildProgram(): " << error_to_string(error) << std::endl;
		check_error(clGetProgramBuildInfo(_program, devices[0], CL_PROGRAM_BUILD_LOG, 0, 0, &length), "");
		build_log = new char[length + 1];
		check_error(clGetProgramBuildInfo(_program, devices[0], CL_PROGRAM_BUILD_LOG, length, build_log, 0), "");
		build_log[length] = '\0';
		std::cerr << build_log << std::endl;
		system("pause");
		delete [] build_log;
		exit(EXIT_FAILURE);
	}
	delete [] source;
	delete [] devices;

	_kernel = clCreateKernel(_program, "raytrace", &error);
	check_error(error, "clCreateKernel()");

	set_resolution(width, height);
}

Renderer::~Renderer()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
	
	clReleaseMemObject(_nodes_mem);
	clReleaseMemObject(_materials_mem);
	clReleaseMemObject(_lights_mem);
	clReleaseMemObject(_image_mem);
	
	clReleaseKernel(_kernel);
	clReleaseProgram(_program);
	clReleaseCommandQueue(_queue);
	clReleaseContext(_context);

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void	Renderer::set_resolution(unsigned int const width, unsigned int const height)
{
	if (texture)
	{
		SDL_DestroyTexture(texture);
		clReleaseMemObject(_image_mem);
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);

	cl_int					error;
	cl_image_format const	format = { CL_RGBA, CL_UNSIGNED_INT8 };
	cl_image_desc const		desc = { CL_MEM_OBJECT_IMAGE2D, width, height, 1, 1, 0, 0, 0 };
	
	_image_mem = clCreateImage(_context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, &format, &desc, 0, &error);
	check_error(error, "clCreateImage()");
}

void		Renderer::_set_buffer(Graphicengine const *ge)
{
	cl_int	error;

	if (_nodes_mem_size < ge->aabbtree._size)
	{
		if (_nodes_mem_size)
			clReleaseMemObject(_nodes_mem);
		_nodes_mem_size = ge->aabbtree._size;
		_nodes_mem = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _nodes_mem_size * sizeof(Node), 0, &error);
		check_error(error, "clCreateBuffer()");
	}
	if (_materials_mem_size < ge->_materials_size)
	{
		if (_materials_mem_size)
			clReleaseMemObject(_materials_mem);
		_materials_mem_size = ge->_materials_size;
		_materials_mem = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _materials_mem_size * sizeof(Material), 0, &error);
		check_error(error, "clCreateBuffer()");
		check_error(clEnqueueWriteBuffer(_queue, _materials_mem, CL_TRUE, 0, ge->_materials_count * sizeof(Material), (void *)ge->_materials, 0, 0, 0), "clEnqueueWriteBuffer()");
	}
	if (_lights_mem_size < ge->_lights_size)
	{
		if (_lights_mem_size)
			clReleaseMemObject(_lights_mem);
		_lights_mem_size = ge->_lights_size;
		_lights_mem = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _lights_mem_size * sizeof(Light), 0, &error);
		check_error(error, "clCreateBuffer()");
	}
	check_error(clEnqueueWriteBuffer(_queue, _nodes_mem, CL_TRUE, 0, ge->aabbtree._size * sizeof(Node), (void *)ge->aabbtree._nodes, 0, 0, 0), "clEnqueueWriteBuffer()");
	check_error(clEnqueueWriteBuffer(_queue, _lights_mem, CL_TRUE, 0, ge->_lights_count * sizeof(Light), (void *)ge->_lights, 0, 0, 0), "clEnqueueWriteBuffer()");
}

void	Renderer::render(Graphicengine const *ge)
{
	Computedcamera	cm;

	_compute_camera(ge->camera, cm);
	_set_buffer(ge);

	check_error(clSetKernelArg(_kernel, 0, sizeof(Computedcamera), &cm), "clSetKernelArg(camera)");
	check_error(clSetKernelArg(_kernel, 1, sizeof(int), &ge->aabbtree._root), "clSetKernelArg(root)");
	check_error(clSetKernelArg(_kernel, 2, sizeof(cl_mem), &_nodes_mem), "clSetKernelArg(nodes)");
	check_error(clSetKernelArg(_kernel, 3, sizeof(cl_mem), &_materials_mem), "clSetKernelArg(materials)");
	check_error(clSetKernelArg(_kernel, 4, sizeof(unsigned int), &ge->_lights_count), "clSetKernelArg(lights_count)");
	check_error(clSetKernelArg(_kernel, 5, sizeof(cl_mem), &_lights_mem), "clSetKernelArg(lights)");
	check_error(clSetKernelArg(_kernel, 6, sizeof(cl_mem), &_image_mem), "clSetKernelArg(image)");

	size_t const	origin[3] = { 0, 0, 0 };
	size_t const	image_size[3] = { ge->camera.resolution[0], ge->camera.resolution[1], 1 };

	check_error(clEnqueueNDRangeKernel(_queue, _kernel, 2, 0, image_size, 0, 0, 0, 0), "clEnqueueNDRangeKernel()");
	clFinish(_queue);

	void			*data;
	int				pitch;

	SDL_LockTexture(texture, 0, &data, &pitch);
	check_error(clEnqueueReadImage(_queue, _image_mem, CL_TRUE, origin, image_size, pitch, 0, data, 0, 0, 0), "clEnqueueReadImage()");
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, 0, 0);
	SDL_RenderPresent(renderer);
}
