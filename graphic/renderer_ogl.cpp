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
#include "renderer_ogl.hpp"

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

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	_glyphstexture = _load_font("cousine-bold.ttf", _glyphs);
	glFinish();

	GLuint	vertexshader;
	GLuint	fragshader;
	GLint	success;
	GLchar	infoLog[512];
	
	char	*source;
	if (!(source = read_file("vertex.glsl", 0)))
		exit(EXIT_FAILURE);

	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &source, 0);
	glCompileShader(vertexshader);
	
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
	if(	!success)
	{
		glGetShaderInfoLog(vertexshader, 512, 0, infoLog);
		std::cout << "error! glCompileShader fails on vexter shader: \n" << infoLog << std::endl;
		exit(EXIT_FAILURE);
	};
	
	delete [] source;
	if (!(source = read_file("frag.glsl", 0)))
		exit(EXIT_FAILURE);
	
	fragshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragshader, 1, &source, 0);
	glCompileShader(fragshader);
	
	glGetShaderiv(fragshader, GL_COMPILE_STATUS, &success);
	if(	!success)
	{
		glGetShaderInfoLog(fragshader, 512, 0, infoLog);
		std::cout << "error! glCompileShader fails on fragment shader: \n" << infoLog << std::endl;
		exit(EXIT_FAILURE);
	};
	
	delete [] source;

	_program = glCreateProgram();
	glAttachShader(_program, vertexshader);
	glAttachShader(_program, fragshader);
	glLinkProgram(_program);

	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(_program, 512, 0, infoLog);
		std::cout << "error! glLinkProgram fails: \n" << infoLog << std::endl;
	}

	glDeleteShader(vertexshader);
	glDeleteShader(fragshader);
	
	glUseProgram(_program);

	glBindFragDataLocation(_program, 0, "color");
	_cameraidx = glGetUniformLocation(_program, "camera");
	_rootidx = glGetUniformLocation(_program, "root");
	_nodesidx = glGetUniformLocation(_program, "nodes");
	_materialsidx = glGetUniformLocation(_program, "materials");
	_lightsnbridx = glGetUniformLocation(_program, "lightsnbr");
	_lightsidx = glGetUniformLocation(_program, "lights");

	glGenBuffers(1, &_nodesbuffer);
	glGenBuffers(1, &_materialsbuffer);
	glGenBuffers(1, &_lightsbuffer);
}

Renderer::~Renderer()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
	
	glDeleteBuffers(1, &_nodesbuffer);
	glDeleteBuffers(1, &_materialsbuffer);
	glDeleteBuffers(1, &_lightsbuffer);
	
	glDeleteTextures(1, &_texture);
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
/*	cl_int	error;

	width = w;
	height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glOrtho(0, w, h, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glFinish();

	clReleaseMemObject(_image_mem);
	_image_mem = clCreateFromGLTexture(_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, _texture, &error);
	check_error(error, "clCreateImage()");*/
}

void		Renderer::_set_buffer(Graphicengine const *ge)
{
	if (_nodes_mem_size < ge->aabbtree._size)
	{
		_nodes_mem_size = ge->aabbtree._size;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _nodesbuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _nodes_mem_size * sizeof(Aabbtree::Node), 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _nodesbuffer);
	}
	if (_materials_mem_size < ge->_materials_size)
	{
		_materials_mem_size = ge->_materials_size;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialsbuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _materials_mem_size * sizeof(Material), (void *)ge->_materials, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, _materialsbuffer);
	}
	if (_lights_mem_size < ge->_lights_size)
	{
		_lights_mem_size = ge->_lights_size;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lightsbuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _lights_mem_size * sizeof(Light), 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, _lightsbuffer);
	}
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _nodesbuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _nodes_mem_size * sizeof(Aabbtree::Node), (void *)ge->aabbtree._nodes);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lightsbuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ge->_lights_count * sizeof(Light), (void *)ge->_lights);
}

void	Renderer::render(Graphicengine const *ge)
{
/*	Computedcamera	cm;

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
	size_t const	image_size[3] = { width, height, 1 };

	glFinish();
	check_error(clEnqueueAcquireGLObjects(_queue, 1, &_image_mem, 0, 0, 0), "clEnqueueAcquireGLObjects()");
	check_error(clEnqueueNDRangeKernel(_queue, _kernel, 2, 0, image_size, 0, 0, 0, 0), "clEnqueueNDRangeKernel()");
	check_error(clEnqueueReleaseGLObjects(_queue, 1, &_image_mem, 0, 0, 0), "clEnqueueReleaseGLObjects()");
	clFinish(_queue);*/

	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(0.0, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f((float)width, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f((float)width, (float)height, 0.0f);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(0.0, (float)height, 0.0f);
	glEnd();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glFinish();
	SDL_GL_SwapWindow(_window);
	glClear(GL_COLOR_BUFFER_BIT);
}
