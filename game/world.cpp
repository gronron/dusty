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

#include <cfloat>
#include <fstream>
#include "common.hpp"
#include "world.hpp"
#include <iostream>

#include "file/file.hpp"
#include "new.hpp"
#include "aabb.hpp"

unsigned int const	CHUNK_SIZE_1D = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

FACTORYREG(World);

#undef far
#undef near

struct		Result : public Raycastcallback
{
	Body	*body;
	int		aabbindex;
	float	near;
	float	far;
	
	Result(Body *bd) : body(bd), aabbindex(-1), near(INFINITY), far(INFINITY) { }

	bool	report_encounter(Body *bd, int const aabbidx, float const nr, float const fr)
	{
		if (bd == body && nr > 0.0f && nr < near)
		{
			aabbindex = aabbidx;
			near = nr;
			far = fr;
		}
		return (true);
	}
};

World::World(Gameengine *g, Replication *r, int const i, short int const t, Entity const *o) : Entity(g, r, i, t, o)
{
	size = 2;
	_init_world();

	shape.size = 1.0f;
	engine->physic->new_body(&body, &shape, this);
	body->dynamic = false;
	body->mass = INFINITY;

	vec<int, 4>	start;
	vec<int, 4>	end;
	start = 0;
	end[0] = 64;
	end[1] = 64;
	end[2] = 1;
	fill(start, end, 1);

	//generate_chunk(chunks);
	
	if (engine->graphic)
	{
		engine->graphic->new_light(&light);
		light->position = 0.0f;
		light->color = { 1.0f, 1.0f, 0.6f };
		light->power = 4000.0f;

		_reduce_world();
	}
}

World::~World()
{
	engine->physic->delete_body(body);
	delete_space(chunks);
}

void	World::tick(float const delta)
{
	float time;
	
	time = 1.0f;
	light->position[1] = (float)sin(time) * 4000.0f;
	light->position[2] = (float)cos(time) * 4000.0f;
}

bool					World::load(char const *filename)
{
	unsigned int const	length = strlen(filename);
	
	if (filename[length - 1] == 'w')
		return (load_dstw(filename));
	else
		return (load_vox(filename));
}

bool				World::load_dstw(char const *filename)
{
	std::ifstream	is(filename);

	if (is.good())
	{
		_clear_world();

		is.read((char *)&size, sizeof(vec<unsigned int, 4>));
		if (chunks)
			delete_space(chunks);
		_init_world();
		unsigned int const	buffer_size = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

		for (unsigned int x = 0; x < size[0]; ++x)
			for (unsigned int y = 0; y < size[1]; ++y)
				for (unsigned int z = 0; z < size[2]; ++z)
					is.read((char *)chunks[x][y][z].blocks[0][0], buffer_size);
		_reduce_world();
		return (true);
	}
	else
	{
		std::cerr << "error! World::load() fails to load: " << filename << std::endl;
		return (false);
	}
}

bool				World::load_vox(char const *filename)
{
	unsigned int	fsize;
	char			*buffer = read_file(filename, &fsize);
	unsigned int	it = 20;

	if (buffer)
	{
		_clear_world();
		if (chunks)
			delete_space(chunks);
		
		if (fsize < 36 || buffer[0] != 'V' || buffer[1] != 'O' || buffer[2] != 'X' || buffer[3] != ' ' || buffer[8] != 'M' || buffer[9] != 'A' || buffer[10] != 'I' || buffer[11] != 'N')
		{
			std::cerr << "error! load_vox() 165 fails to load: " << filename << std::endl;
			delete [] buffer;
			return (false);
		}
		
		it += *(unsigned int *)(buffer + 12);

		if (buffer[it] != 'S' || buffer[it + 1] != 'I' || buffer[it + 2] != 'Z' || buffer[it + 3] != 'E')
		{
			std::cerr << "error! load_vox() 175 fails to load: " << filename << std::endl;
			delete [] buffer;
			return (false);
		}
		
		it += 12;
		size = *(vec<unsigned int, 3> *)(buffer + it) / (unsigned int)CHUNK_SIZE + 1U;
		it += 12;

		_init_world();

		if (buffer[it] != 'X' || buffer[it + 1] != 'Y' || buffer[it + 2] != 'Z' || buffer[it + 3] != 'I')
		{
			std::cerr << "error! load_vox() 188 fails to load: " << filename << std::endl;
			delete [] buffer;
			return (false);
		}
		it += 12;

		unsigned int const	voxelcount = *(unsigned int *)(buffer + it);
		std::cout << voxelcount << std::endl;
		
		for (unsigned int i = 0; (it += 4) < fsize && i < voxelcount; ++i)
		{
			vec<unsigned char, 4> const	data = *(vec<unsigned char, 4> *)(buffer + it);
			vec<unsigned char, 4> const	wl = data / (unsigned char)CHUNK_SIZE;
			vec<unsigned char, 4> const	cl = data % (unsigned char)CHUNK_SIZE;
			chunks[wl[0]][wl[1]][wl[2]].blocks[cl[0]][cl[1]][cl[2]] = data[3];
		}

		_reduce_world();
		
		/*if ((fsize - it) >= 1036 && buffer[it] == 'R' && buffer[it + 1] == 'G' && buffer[it + 2] == 'B' && buffer[it + 3] == 'A')
		{
			it += 12;
			for (unsigned int i = 1; i < 255; ++i)
			{
				engine->graphic->_materials[i].color = (vec<float, 3>)*(vec<unsigned char, 3> *)(buffer + it) / 255.0f;
				engine->graphic->_materials[i].transparency = 0.0f;
				engine->graphic->_materials[i].refraction = 0.0f;
				engine->graphic->_materials[i].reflection = 0.0f;
				engine->graphic->_materials[i].shining = 0.0f;
				it += 4;
			}
			engine->graphic->_materials_size = 256;
		}*/

		delete [] buffer;
		
		return (true);
	}
	else
	{
		std::cerr << "error! load_vox() 225 fails to load: " << filename << std::endl;
		return (false);
	}
}

bool				World::save(char const *filename)
{
	std::ofstream	os(filename);

	if (os.good())
	{
		os.write((char *)&size, sizeof(vec<unsigned int, 4>));
		unsigned int const	buffer_size = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

		for (unsigned int x = 0; x < size[0]; ++x)
			for (unsigned int y = 0; y < size[1]; ++y)
				for (unsigned int z = 0; z < size[2]; ++z)
					os.write((char *)chunks[x][y][z].blocks[0][0], buffer_size);
		return (true);
	}
	else
	{
		std::cerr << "fail to save world" << std::endl;
		return (false);
	}
}

void					World::fill(vec<int, 4> const &start, vec<int, 4> const &end, char const value)
{
	vec<int, 4> const	bottom = vmin(start, end);
	vec<int, 4> const	top = vmax(start, end);
	
	for (int k = bottom[0]; k < top[0]; ++k)
	{
		int const	ck = k / CHUNK_SIZE;
		int const	bk = k % CHUNK_SIZE;
		for (int j = bottom[1]; j < top[1]; ++j)
		{
			int const	cj = j / CHUNK_SIZE;
			int const	bj = j % CHUNK_SIZE;
			for (int i = bottom[2]; i < top[2]; ++i)
			{
				int const	ci = i / CHUNK_SIZE;
				int const	bi = i % CHUNK_SIZE;
				chunks[ck][cj][ci].blocks[bk][bj][bi] = value;
			}
		}
	}
}

bool	World::create_block(Ray const &ray, char const value)
{
	Result	result(body);

	engine->physic->raycast_through(ray, &result, false, true);
	if (result.aabbindex != -1)
	{
		vec<float, 4>	normal;
		intersect_rayaabb_n(ray, engine->physic->_statictree._nodes[result.aabbindex].aabb, result.near, result.far, normal);
		
		vec<float, 4> const	position = ray.origin + ray.direction * (result.near - FLT_EPSILON) + normal * 0.5f;
		vec<int, 3> const	wp = (vec<int, 3>)vfloor(position / (float)CHUNK_SIZE);
		vec<int, 3> const	cp = (vec<int, 3>)vfloor(position - (vec<float, 4>)(wp * CHUNK_SIZE));

		if (wp >= 0 && wp < (vec<int, 4>)size && cp >= 0 && cp < CHUNK_SIZE && !chunks[wp[0]][wp[1]][wp[2]].blocks[cp[0]][cp[1]][cp[2]])
		{
			chunks[wp[0]][wp[1]][wp[2]].blocks[cp[0]][cp[1]][cp[2]] = value;

			vec<float, 4> const	fp = (vec<float, 4>)(wp) * CHUNK_SIZE;
			_clear_chunk(chunks[wp[0]][wp[1]][wp[2]], fp);
			_reduce_chunk(chunks[wp[0]][wp[1]][wp[2]], fp);
			return (true);
		}
	}
	return (false);
}

bool	World::destroy_block(Ray const &ray)
{
	Result	result(body);

	engine->physic->raycast_through(ray, &result, false, true);
	if (result.aabbindex != -1)
	{
		vec<float, 4>	normal;
		intersect_rayaabb_n(ray, engine->physic->_statictree._nodes[result.aabbindex].aabb, result.near, result.far, normal);
		
		vec<float, 4> const	position = ray.origin + ray.direction * (result.near + FLT_EPSILON) - normal * 0.5f;
		vec<int, 3> const	wp = (vec<int, 3>)vfloor(position / (float)CHUNK_SIZE);
		vec<int, 3> const	cp = (vec<int, 3>)vfloor(position - (vec<float, 4>)(wp * CHUNK_SIZE));

		if (wp >= 0 && wp < (vec<int, 4>)size && cp >= 0 && cp < CHUNK_SIZE)
		{
			chunks[wp[0]][wp[1]][wp[2]].blocks[cp[0]][cp[1]][cp[2]] = 0;

			vec<float, 4> const	fp = (vec<float, 4>)(wp) * CHUNK_SIZE;
			_clear_chunk(chunks[wp[0]][wp[1]][wp[2]], fp);
			_reduce_chunk(chunks[wp[0]][wp[1]][wp[2]], fp);
			return (true);
		}
	}
	return (false);
}

void					_search_ppr(World::Chunk &chunk, vec<unsigned int, 4> const &begin, vec<unsigned int, 4> &end)
{
	unsigned char const	blocks = chunk.blocks[begin[0]][begin[1]][begin[2]];

	for (end[2] = begin[2] + 1; end[2] < CHUNK_SIZE; ++end[2])
		if (chunk.blocks[begin[0]][begin[1]][end[2]] != blocks || chunk.physicids[begin[0]][begin[1]][end[2]] >= 0)
			break;

	end[1] = CHUNK_SIZE;
	for (unsigned int i = begin[1] + 1; i < end[1]; ++i)
		for (unsigned int j = begin[2]; j < end[2]; ++j)
			if (chunk.blocks[begin[0]][i][j] != blocks || chunk.physicids[begin[0]][i][j] >= 0)
			{
				end[1] = i;
				break;
			}

	for (end[0] = begin[0] + 1; end[0] < CHUNK_SIZE; ++end[0])
		for (unsigned int i = begin[1]; i < end[1]; ++i)
			for (unsigned int j = begin[2]; j < end[2]; ++j)
				if (chunk.blocks[end[0]][i][j] != blocks || chunk.physicids[end[0]][i][j] >= 0)
					return;
}

void	World::_reduce_chunk(Chunk &chunk, vec<float, 4> const &position)
{
	for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
		for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
			for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
			{
				if (chunk.physicids[x][y][z] < 0 && chunk.blocks[x][y][z])
				{
					vec<unsigned int, 4> begin = { x, y, z, 0 };
					vec<unsigned int, 4> end;

					_search_ppr(chunk, begin, end);

					Aabb	aabb;

					aabb.bottom = { (float)x, (float)y, (float)z, 0.0f };
					aabb.bottom += position;
					aabb.top = position + (vec<float, 4>)end;

					int const	physicids = engine->physic->add_aabb(body, aabb);
					int const	graphicids = engine->graphic->aabbtree.add_saabb(aabb, chunk.blocks[x][y][z]);

					for (unsigned int i = x; i < end[0]; ++i)
						for (unsigned int j = y; j < end[1]; ++j)
							for (unsigned int k = z; k < end[2]; ++k)
							{
								chunk.physicids[i][j][k] = physicids;
								chunk.graphicids[i][j][k] = graphicids;
							}
				}
			}
}

void	World::_reduce_world()
{
	for (unsigned int x = 0; x < size[0]; ++x)
		for (unsigned int y = 0; y < size[1]; ++y)
			for (unsigned int z = 0; z < size[2]; ++z)
				_reduce_chunk(chunks[x][y][z], { (float)x * CHUNK_SIZE, (float)y * CHUNK_SIZE, (float)z * CHUNK_SIZE, 0.0f });
}

void	World::_init_world()
{
	chunks = new_space<Chunk>(size[0], size[1], size[2]);
	for (unsigned int x = 0; x < size[0]; ++x)
		for (unsigned int y = 0; y < size[1]; ++y)
			for (unsigned int z = 0; z < size[2]; ++z)
				for (unsigned int i = 0; i < CHUNK_SIZE_1D; ++i)
				{
					chunks[x][y][z].blocks[0][0][i] = 0;
					chunks[x][y][z].physicids[0][0][i] = -1;
					chunks[x][y][z].graphicids[0][0][i] = -1;
				}
}

void	World::_clear_chunk(Chunk &chunk, vec<float, 4> const &position)
{
	for (unsigned int i = 0; i < CHUNK_SIZE_1D; ++i)
		if (chunk.physicids[0][0][i] != -1)
		{
			engine->physic->remove_aabb(body, chunk.physicids[0][0][i]);
			engine->graphic->aabbtree.remove_aabb(chunk.graphicids[0][0][i]);
			chunk.physicids[0][0][i] = -1;
			chunk.graphicids[0][0][i] = -1;
		}
}

void	World::_clear_world()
{
	engine->physic->remove_aabbs(body);
	for (unsigned int x = 0; x < size[0]; ++x)
		for (unsigned int y = 0; y < size[1]; ++y)
			for (unsigned int z = 0; z < size[2]; ++z)
				for (unsigned int i = 0; i < CHUNK_SIZE_1D; ++i)
					if (chunks[x][y][z].physicids[0][0][i] != -1)
					{
						engine->graphic->aabbtree.remove_aabb(chunks[x][y][z].graphicids[0][0][i]);
						chunks[x][y][z].physicids[0][0][i] = -1;
						chunks[x][y][z].graphicids[0][0][i] = -1;
					}
}
/*
void	World::explosion(vec<float, 4> const &position, float const radius, float const power)
{
	//get block
	//fill particle and remove block
	//create particles systeme
}
*/
