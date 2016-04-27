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
#include <CL/cl_gl.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <iostream>
#include <cstdlib>
#include "opencl_error.hpp"
#include "new.hpp"
#include "math/vec_util.hpp"
#include "file/file.hpp"
#include "graphicengine.hpp"
#include "renderer.hpp"

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
	cl_int			error;
	cl_uint			platforms_count;
	cl_platform_id	*platforms;
	size_t			devices_count;
	cl_device_id	*devices;
	SDL_SysWMinfo 	info;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "error! SDL_Init()" << std::endl;
		exit(EXIT_FAILURE);
	}
	_window = SDL_CreateWindow("dusty", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
	_glcontext = SDL_GL_CreateContext(_window);
	SDL_GL_SetSwapInterval(0);

	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo(_window, &info))
	{
		std::cerr << "error! SDL_GetWindowWMInfo()" << std::endl;
		exit(EXIT_FAILURE);
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glViewport(0, 0, width, height);
	glOrtho(0, width, height, 0, -1000, 1000);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	_glyphstexture = _load_font("cousine-regular.ttf", _glyphs);
	glFinish();
	
	check_error(clGetPlatformIDs(0, 0, &platforms_count), "clGetPlatformIDs()");
	platforms = new cl_platform_id[platforms_count];
	check_error(clGetPlatformIDs(platforms_count, platforms, 0), "clGetPlatformIDs()");
	
	cl_context_properties const	properties[] =
	{
		CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0],
		CL_GL_CONTEXT_KHR, (cl_context_properties)_glcontext,
		CL_WGL_HDC_KHR, (cl_context_properties)info.info.win.hdc,
		0
	};

	_context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, 0, 0, &error);
	check_error(error, "clCreateContextFromType()");
	delete [] platforms;

	check_error(clGetContextInfo(_context, CL_CONTEXT_DEVICES, 0, 0, &devices_count), "clGetContextInfo()");
	devices = new cl_device_id[devices_count];
	check_error(clGetContextInfo(_context, CL_CONTEXT_DEVICES, devices_count, devices, 0), "clGetContextInfo()");

	char	device_vendor[64];
	char	device_name[64];
	check_error(clGetDeviceInfo(devices[0], CL_DEVICE_VENDOR, 64, device_vendor, 0), "clGetDeviceInfo()");
	check_error(clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 64, device_name, 0), "clGetDeviceInfo()");
	std::cout << "Device selected:\n\t" << device_vendor << " - " << device_name << std::endl;

	/*_queue = clCreateCommandQueueWithProperties(_context, devices[0], 0, &error); //OpenCL 2.0
	check_error(error, "clCreateCommandQueueWithProperties()");*/
	_queue = clCreateCommandQueue(_context, devices[0], 0, &error);
	check_error(error, "clCreateCommandQueue()");

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
		check_error(clGetProgramBuildInfo(_program, devices[0], CL_PROGRAM_BUILD_LOG, 0, 0, &length), "clGetProgramBuildInfo()");
		build_log = new char[length + 1];
		check_error(clGetProgramBuildInfo(_program, devices[0], CL_PROGRAM_BUILD_LOG, length, build_log, 0), "clGetProgramBuildInfo()");
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
	
	_camera_mem = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(Computedcamera), 0, &error);
	_image_mem = clCreateFromGLTexture(_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _texture, &error);
	check_error(error, "clCreateImage()");
}

Renderer::~Renderer()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
	
	clReleaseMemObject(_camera_mem);
	clReleaseMemObject(_nodes_mem);
	clReleaseMemObject(_materials_mem);
	clReleaseMemObject(_lights_mem);
	clReleaseMemObject(_image_mem);
	
	clReleaseKernel(_kernel);
	clReleaseProgram(_program);
	clReleaseCommandQueue(_queue);
	clReleaseContext(_context);

	glDeleteTextures(1, &_texture);
	SDL_GL_DeleteContext(_glcontext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void	Renderer::set_resolution(unsigned int const w, unsigned int const h)
{
	cl_int	error;

	width = w;
	height = h;
	glViewport(0, 0, width, height);
	glOrtho(0, width, height, 0, -1, 1);

	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glFinish();

	clReleaseMemObject(_image_mem);
	_image_mem = clCreateFromGLTexture(_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _texture, &error);
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
		_nodes_mem = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _nodes_mem_size * sizeof(Aabbtree::Node), 0, &error);
		check_error(error, "clCreateBuffer()");
	}
	if (_materials_mem_size < ge->_materials_size)
	{
		if (_materials_mem_size)
			clReleaseMemObject(_materials_mem);
		_materials_mem_size = ge->_materials_size;
		_materials_mem = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _materials_mem_size * sizeof(Material), 0, &error);
		check_error(error, "clCreateBuffer()");
		check_error(clEnqueueWriteBuffer(_queue, _materials_mem, CL_FALSE, 0, ge->_materials_count * sizeof(Material), (void *)ge->_materials, 0, 0, 0), "clEnqueueWriteBuffer()");
	}
	if (_lights_mem_size < ge->_lights_size)
	{
		if (_lights_mem_size)
			clReleaseMemObject(_lights_mem);
		_lights_mem_size = ge->_lights_size;
		_lights_mem = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, _lights_mem_size * sizeof(Light), 0, &error);
		check_error(error, "clCreateBuffer()");
	}
	check_error(clEnqueueWriteBuffer(_queue, _nodes_mem, CL_FALSE, 0, ge->aabbtree._size * sizeof(Aabbtree::Node), (void *)ge->aabbtree._nodes, 0, 0, 0), "clEnqueueWriteBuffer()");
	check_error(clEnqueueWriteBuffer(_queue, _lights_mem, CL_FALSE, 0, ge->_lights_count * sizeof(Light), (void *)ge->_lights, 0, 0, 0), "clEnqueueWriteBuffer()");
}

void	Renderer::render(Graphicengine const *ge)
{
	Computedcamera	cm;

	_compute_camera(ge->camera, cm);
	_set_buffer(ge);

	check_error(clEnqueueWriteBuffer(_queue, _camera_mem, CL_FALSE, 0, sizeof(Computedcamera), (void *)&cm, 0, 0, 0), "clEnqueueWriteBuffer()");
	check_error(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_camera_mem), "clSetKernelArg(camera)");
	check_error(clSetKernelArg(_kernel, 1, sizeof(int), &ge->aabbtree._root), "clSetKernelArg(root)");
	check_error(clSetKernelArg(_kernel, 2, sizeof(cl_mem), &_nodes_mem), "clSetKernelArg(nodes)");
	check_error(clSetKernelArg(_kernel, 3, sizeof(cl_mem), &_materials_mem), "clSetKernelArg(materials)");
	check_error(clSetKernelArg(_kernel, 4, sizeof(unsigned int), &ge->_lights_count), "clSetKernelArg(lights_count)");
	check_error(clSetKernelArg(_kernel, 5, sizeof(cl_mem), &_lights_mem), "clSetKernelArg(lights)");
	check_error(clSetKernelArg(_kernel, 6, sizeof(cl_mem), &_image_mem), "clSetKernelArg(image)");

	size_t const	origin[3] = { 0, 0, 0 };
	size_t const	image_size[3] = { ge->camera.resolution[0], ge->camera.resolution[1], 1 };

	glFinish();
	check_error(clEnqueueAcquireGLObjects(_queue, 1, &_image_mem, 0, 0, 0), "clEnqueueAcquireGLObjects()");
	check_error(clEnqueueNDRangeKernel(_queue, _kernel, 2, 0, image_size, 0, 0, 0, 0), "clEnqueueNDRangeKernel()");
	check_error(clEnqueueReleaseGLObjects(_queue, 1, &_image_mem, 0, 0, 0), "clEnqueueReleaseGLObjects()");
	clFinish(_queue);

	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(0.0, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f((float)width, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f((float)width, (float)height, 0.0f);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(0.0, (float)height, 0.0f);
	glEnd();

	glFinish();
	SDL_GL_SwapWindow(_window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
