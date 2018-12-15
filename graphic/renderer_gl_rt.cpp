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

#include "renderer_gl.hpp"

#define GLEW_STATIC
#include <glew.h>

#include "math/vec_util.hpp"
#include "graphicengine.hpp"

struct				Computedcamera
{
	vec<float, 4>	position;
	vec<float, 4>	forward;
	vec<float, 4>	right;
	vec<float, 4>	up;
	vec<float, 2>	half_resolution;
};

static inline void	_compute_camera(Camera const &camera, Computedcamera &cm)
{
	cm.position = camera.position;
	cm.forward = (camera.direction * (float)camera.resolution[0]) / (tan(camera.fov / 2.0f) * 2.0f);
	cm.right = camera.right;
	cm.up = vunit<float>(vcross(camera.direction, camera.right));
	cm.half_resolution = (vec<float, 2>)camera.resolution / 2.0f;
}

RTRenderer::RTRenderer()
{
}

RTRenderer::~RTRenderer()
{
	glDeleteBuffers(1, &_nodes_buffer);
	glDeleteBuffers(1, &_matrices_buffer);
	glDeleteBuffers(1, &_materials_buffer);
	glDeleteBuffers(1, &_lights_buffer);
}

void	RTRenderer::_init()
{
	_program = _build_program("raytracer_vertex.glsl", "raytracer_frag.glsl");
	glUseProgram(_program);

	glBindFragDataLocation(_program, 0, "color");
	_check_error(__LINE__);

	_camera_idx = glGetUniformBlockIndex(_program, "_camera");
	glUniformBlockBinding(_program, _camera_idx, 1);
	_check_error(__LINE__);

	_nodes_idx = glGetProgramResourceIndex(_program, GL_SHADER_STORAGE_BLOCK, "_nodes");
	glShaderStorageBlockBinding(_program, _nodes_idx, 2);
	_check_error(__LINE__);

	_matrices_idx = glGetProgramResourceIndex(_program, GL_SHADER_STORAGE_BLOCK, "_matrices");
	glShaderStorageBlockBinding(_program, _matrices_idx, 3);
	_check_error(__LINE__);

	_materials_idx = glGetProgramResourceIndex(_program, GL_SHADER_STORAGE_BLOCK, "_materials");
	glShaderStorageBlockBinding(_program, _materials_idx, 4);
	_check_error(__LINE__);

	_lights_nbr_idx = glGetUniformLocation(_program, "lights_number");
	_check_error(__LINE__);

	_lights_idx = glGetProgramResourceIndex(_program, GL_SHADER_STORAGE_BLOCK, "_lights");
	glShaderStorageBlockBinding(_program, _lights_idx, 6);
	_check_error(__LINE__);

	glGenBuffers(1, &_camera_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _camera_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Computedcamera), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, _camera_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	_check_error(__LINE__);

	glGenBuffers(1, &_nodes_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _nodes_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _nodes_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	_check_error(__LINE__);

	glGenBuffers(1, &_matrices_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _matrices_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _matrices_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	_check_error(__LINE__);

	glGenBuffers(1, &_materials_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materials_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, _materials_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	_check_error(__LINE__);

	glGenBuffers(1, &_lights_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lights_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, _lights_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	_check_error(__LINE__);

	float const	vertices[] =
	{
    	-1.0f,	1.0f,	0.0f,
    	1.0f,	1.0f,	0.0f,
     	-1.0f,	-1.0f,	0.0f,
     	1.0f,	1.0f,	0.0f,
     	1.0f,	-1.0f,	0.0f,
     	-1.0f,	-1.0f,	0.0f
	};

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	_check_error(__LINE__);
}

void	RTRenderer::_set_buffer(Graphicengine const *ge)
{
	Computedcamera	cm;

	_compute_camera(ge->camera, cm);
	glBindBuffer(GL_UNIFORM_BUFFER, _camera_buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Computedcamera), (void *)&cm);

	if (_nodes_mem_size < ge->oatree._size)
	{
		_nodes_mem_size = ge->oatree._size;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _nodes_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _nodes_mem_size * sizeof(OrderedAabbNode), nullptr, GL_DYNAMIC_DRAW);
	}
	if (_matrices_mem_size < ge->_matrices.size)
	{
		_matrices_mem_size = ge->_matrices.size;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _matrices_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _matrices_mem_size * sizeof(mat3), nullptr, GL_DYNAMIC_DRAW);
	}
	if (_materials_mem_size < ge->_materials.size)
	{
		_materials_mem_size = ge->_materials.size;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materials_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _materials_mem_size * sizeof(Material), (void *)ge->_materials.data, GL_STATIC_DRAW);
	}
	if (_lights_mem_size < ge->_lights_size)
	{
		_lights_mem_size = ge->_lights_size;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lights_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _lights_mem_size * sizeof(Light), nullptr, GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _nodes_buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ge->oatree._count * sizeof(OrderedAabbNode), (void *)ge->oatree._nodes);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _matrices_buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ge->_matrices.number * sizeof(mat3), (void *)ge->_matrices.data);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _lights_buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ge->_lights_count * sizeof(Light), (void *)ge->_lights);

	glProgramUniform1ui(_program, _lights_nbr_idx, ge->_lights_count);

	_check_error(__LINE__);
}

void	RTRenderer::_render()
{
	glUseProgram(_program);
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	_check_error(__LINE__);
}
