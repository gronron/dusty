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

#include <filesystem>
#include <iostream>
#include <cstdlib>

#define GLEW_STATIC
#include <glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

FlatRenderer::FlatRenderer()
{

}

FlatRenderer::~FlatRenderer()
{
	for (unsigned int i = 0; i < _texture_ids.number; ++i)
		glDeleteTextures(1, &_texture_ids[i]);
}

void FlatRenderer::_init()
{
	for (auto const & file : std::filesystem::directory_iterator("assets/textures"))
	{
		int	width;
		int	height;
		int format;
		unsigned char *	pixels = stbi_load(file.path().string().c_str(), &width, &height, &format, 0);

		if (pixels != nullptr)
		{
			GLenum const gl_format = [&]() -> GLenum
			{
				switch (format)
				{
					case 1: return GL_LUMINANCE;
					case 2: return GL_LUMINANCE_ALPHA;
					case 3: return GL_RGB;
					case 4: return GL_RGBA;
					default: return GL_RGBA;
				};
			}();

			unsigned int &	texture_id = _texture_ids.allocate();

			glGenTextures(1, &texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, pixels);

			stbi_image_free(pixels);
		}
		else
			std::cerr << "warning! stbi_load() fails to load: " << file.path() << std::endl;
	}

	_program = _build_program("flat_vertex.glsl", "flat_frag.glsl");
	glUseProgram(_program);

	glBindFragDataLocation(_program, 0, "color");

	GLint texture_loc = glGetUniformLocation(_program, "_texture");

	glUniform1i(texture_loc, 0);

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	_check_error(__LINE__);
}

void	FlatRenderer::_set_buffer(Graphicengine const *ge)
{
	unsigned int trianges_number = 0;
	for (const auto & vertices : _vertices)
		trianges_number += vertices.second.number;

	if (_mem_size < trianges_number)
	{
		_mem_size = trianges_number;
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, _mem_size * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	unsigned int offset = 0;
	for (const auto & vertices : _vertices)
	{
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertices.second.number * sizeof(Vertex), (void *)vertices.second.data);
		offset += vertices.second.number * sizeof(Vertex);
	}

	_check_error(__LINE__);
}

void	FlatRenderer::_render()
{
	glUseProgram(_program);
	glBindVertexArray(_vao);

	unsigned int offset = 0;
	for (auto & vertices : _vertices)
	{
		if (vertices.second.number > 0)
		{
			glBindTexture(GL_TEXTURE_2D, vertices.first);
			glDrawArrays(GL_TRIANGLES, offset, vertices.second.number);
			offset += vertices.second.number;
			vertices.second.clear();
		}
	}

	glBindVertexArray(0);

	_check_error(__LINE__);
}
