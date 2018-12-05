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

#include "renderer_gl.hpp"

#include <iostream>
#include <cstdlib>

#define GLEW_STATIC
#include <glew.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include "new.hpp"
#include "file/file.hpp"
#include "graphicengine.hpp"

void	_check_error(int const line)
{
	GLenum const error = glGetError();

	if (error == GL_NO_ERROR)
		return;

	std::cerr << line << " ";

	switch (error)
	{
	case GL_INVALID_ENUM:
		std::cerr << "GL_INVALID_ENUM" << std::endl;
		break;
	case GL_INVALID_VALUE:
		std::cerr << "GL_INVALID_VALUE" << std::endl;
		break;
	case GL_INVALID_OPERATION:
		std::cerr << "GL_INVALID_OPERATION" << std::endl;
		break;
	case GL_STACK_OVERFLOW:
		std::cerr << "GL_STACK_OVERFLOW" << std::endl;
		break;
	case GL_STACK_UNDERFLOW:
		std::cerr << "GL_STACK_UNDERFLOW" << std::endl;
		break;
	case GL_OUT_OF_MEMORY:
		std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
		break;
	case GL_TABLE_TOO_LARGE:
		std::cerr << "GL_TABLE_TOO_LARGE" << std::endl;
		break;
	default:
		std::cerr << "default" << std::endl;
		break;
	}
}

unsigned int	_build_program(char const * const vertex_name, char const * const frag_name)
{
	GLuint	program;
	char	*source;
	GLuint	vertexshader;
	GLuint	fragshader;
	GLint	success;
	GLchar	infoLog[4096];

	if (!(source = read_file(vertex_name, 0)))
		exit(EXIT_FAILURE);

	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &source, 0);
	glCompileShader(vertexshader);

	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(vertexshader, 4096, 0, infoLog);
	std::cout << infoLog << std::endl;
	if (!success)
	{
		std::cout << "error! glCompileShader() fails on vertex shader: " << vertex_name << std::endl;
		exit(EXIT_FAILURE);
	};

	delete [] source;

	if (!(source = read_file(frag_name, 0)))
		exit(EXIT_FAILURE);

	fragshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragshader, 1, &source, 0);
	glCompileShader(fragshader);

	glGetShaderiv(fragshader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(fragshader, 4096, 0, infoLog);
	std::cout << infoLog << std::endl;
	if (!success)
	{
		std::cout << "error! glCompileShader() fails on fragment shader: " << frag_name << std::endl;
		exit(EXIT_FAILURE);
	};

	delete [] source;

	program = glCreateProgram();
	glAttachShader(program, vertexshader);
	glAttachShader(program, fragshader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(program, 4096, 0, infoLog);
		std::cout << "error! glLinkProgram fails: \n" << infoLog << std::endl;
	}

	glDeleteShader(vertexshader);
	glDeleteShader(fragshader);

	return (program);
}

Renderer::Renderer(unsigned int const w, unsigned int const h, bool const fullscreen) : width(w), height(h)
{
	SDL_SysWMinfo 	info;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "error! SDL_Init()" << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	_window = (void *)SDL_CreateWindow("dusty", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
	_glcontext = (void *)SDL_GL_CreateContext((SDL_Window *)_window);
	SDL_GL_SetSwapInterval(0);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "error! glewInit()" << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo((SDL_Window *)_window, &info))
	{
		std::cerr << "error! SDL_GetWindowWMInfo()" << std::endl;
		exit(EXIT_FAILURE);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, width, height);

	_rt_renderer._init();
	_text_renderer._init();
}

Renderer::~Renderer()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);

	SDL_GL_DeleteContext((SDL_GLContext)_glcontext);
	SDL_DestroyWindow((SDL_Window *)_window);
	SDL_Quit();
}

void	Renderer::set_fullscreen(bool const fullscreen)
{
	SDL_SetWindowFullscreen((SDL_Window *)_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void	Renderer::set_resolution(unsigned int const w, unsigned int const h)
{
	width = w;
	height = h;

	glViewport(0, 0, width, height);
}

void	Renderer::render(Graphicengine const *ge)
{
	_rt_renderer._set_buffer(ge);
	_text_renderer._set_buffer(ge);

	_rt_renderer._render();
	_text_renderer._render();

	glFinish();

	SDL_GL_SwapWindow((SDL_Window *)_window);
	glClear(GL_COLOR_BUFFER_BIT);
}
