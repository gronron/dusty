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

#include "shader.hpp"

static char			*read_file(char const *filename)
{
	std::ifstream	is(filename);
	int				length;
	char			*buffer = 0;

	if (is)
	{
		is.seekg(0, is.end);
		length = is.tellg();
		is.seekg(0, is.beg);
		buffer = new char[length];

		if (!is.read(buffer, length))
		{
			std::cerr << "create_shader() can't read file: " << filename << std::endl;
			delete [] buffer;
			buffer = 0;
		}
		is.close();
	}
	else
		std::cerr << "create_shader() can't open file: " << filename << std::endl;

	return (buffer);
}

GLuint		create_shader(char const *filename, GLenum const shadertype)
{
	char	*buffer
	GLuint	shader;
	GLint	error;
	GLchar	info[512];

	if (!buffer = read_file(filename))
		return (0);

	shader = glCreateShader(shadertype);
	glShaderSource(shader, 1, &buffer, 0);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &error);
	if (error)
	{
		glGetShaderInfoLog(shader, 512, 0, info);
		std::cerr << "create_shader(): compilaion failed on file " << filename << ":\n\t" << info << std::endl;
		glDeleteShader(shader);
		shader = 0;
	}

	delete [] buffer;
	return (shader);
}

void	delete_shader(GLuint shader)
{
	glDeleteShader(shader);
}

Shaderprogram::Shaderprogram()
{
	
}

Shaderprogram::Shaderprogram(GLuint const vertex, GLuint const geometry, GLuint const fragment)
{
	GLint	error;
	GLchar	info[512];

	program = glCreateProgram();

	if (vertex)
		glAttachShader(program, vertex);
	if (geometry)
		glAttachShader(program, geometry);
	if (fragment)
		glAttachShader(program, fragment);
	
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &error);
	if (error)
	{
		glGetProgramInfoLog(program, 512, 0, info);
		std::cerr << "create_programshader(): linking failed:\n\t" << info << std::endl;
	}
}

Shaderprogram::~Shaderprogram()
{
	
}

bool	Shaderprogram::is_good()
{
	
}

void	Shaderprogram::use()
{
	glUseProgram(program);
}
