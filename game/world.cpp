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

#include <fstream>
#include "common.hpp"
#include "world.hpp"
#include <iostream>

#include "new.hpp"
#include "aabb.hpp"

#include "particlesystem.hpp"

FACTORYREG(World);

#undef far
#undef near

struct		Result
{
	int		bodyindex;
	int		aabbindex;
	float	near;
	float	far;
	
	bool	callback(int const aabbidx, int const bdidx, float const nr, float const fr)
	{
		if (/*bdidx == bodyindex && */nr > 0.0f && nr < near)
		{
			aabbindex = aabbidx;
			near = nr;
			far = fr;
		}
		return (true);
	}
};

World::World(Gameengine *g, Replication *r, int const i, short int const t, Actor const *o) : Actor(g, r, i, t, o)
{
	size = 2;
	chunks = new_space<Chunk>(2, 2, 2);
	
	vec<int, 4>	start;
	vec<int, 4>	end;
	start = 0;
	end = 64;
	fill(start, end, 0);
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

		_cull_world();
		
		vec<float, 3> v;
		v = 16.0f;
		engine->graphic->add_animation(new Particlesystem(engine->graphic, "player", v));
	}
}

World::~World()
{
	delete_space(chunks);
}

void	World::tick(float const delta)
{
	time += delta;
	
	time = 1.0f;
	light->position[1] = (float)sin(time) * 4000.0f;
	light->position[2] = (float)cos(time) * 4000.0f;
}

bool				World::load(char const *filename)
{
	std::ifstream	is(filename);

	if (is.good())
	{
		is.read((char *)&size, sizeof(vec<unsigned int, 4>));
		if (chunks)
			delete_space(chunks);
		chunks = new_space<Chunk>(size[0], size[1], size[2]);

		unsigned int const	buffer_size = size[0] * size[1] * size[2] * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
		is.read((char *)chunks[0][0], buffer_size);
		_cull_world();
		return (true);
	}
	else
	{
		std::cerr << "fail to load world" << std::endl;
		return (false);
	}
}

bool				World::save(char const *filename)
{
	std::ofstream	os(filename);

	if (os.good())
	{
		os.write((char *)&size, sizeof(vec<unsigned int, 4>));

		unsigned int const	buffer_size = size[0] * size[1] * size[2] * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
		os.write((char *)chunks[0][0], buffer_size);
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
	Result	result = { id, -1, INFINITY, INFINITY };
	
	engine->graphic->aabbtree.raycast(ray, &result, &Result::callback);
	if (result.aabbindex != -1)
	{
		vec<float, 4>	normal;
		intersect_rayaabb_n(ray, engine->graphic->aabbtree._nodes[result.aabbindex].aabb, result.near, result.far, normal);

		vec<float, 4> const	position = engine->graphic->aabbtree._nodes[result.aabbindex].aabb.bottom + normal;
		vec<float, 4> const	wp = vcall(floor, position / (float)CHUNK_SIZE);
		vec<float, 4> const	cp = vcall(round, position - wp * (float)CHUNK_SIZE);

		if (!chunks[(int)wp[0]][(int)wp[1]][(int)wp[2]].blocks[(int)cp[0]][(int)cp[1]][(int)cp[2]])
		{
			chunks[(int)wp[0]][(int)wp[1]][(int)wp[2]].blocks[(int)cp[0]][(int)cp[1]][(int)cp[2]] = value;

			Aabb	aabb;
			aabb.bottom = position;
			aabb.top = aabb.bottom + 1.0f;
			engine->graphic->aabbtree.add_saabb(aabb, value);
			return (true);
		}
	}
	return (false);
}

bool	World::destroy_block(Ray const &ray)
{
	Result	result = { id, -1, INFINITY, INFINITY };
	
	engine->graphic->aabbtree.raycast(ray, &result, &Result::callback);
	if (result.aabbindex != -1)
	{
		vec<float, 4> const	position = engine->graphic->aabbtree._nodes[result.aabbindex].aabb.bottom;
		vec<float, 4> const	wp = vcall(floor, position / (float)CHUNK_SIZE);
		vec<float, 4> const	cp = vcall(round, position - wp * (float)CHUNK_SIZE);
		
		if (chunks[(int)wp[0]][(int)wp[1]][(int)wp[2]].blocks[(int)cp[0]][(int)cp[1]][(int)cp[2]])
		{
			chunks[(int)wp[0]][(int)wp[1]][(int)wp[2]].blocks[(int)cp[0]][(int)cp[1]][(int)cp[2]] = 0;
			engine->graphic->aabbtree.remove_aabb(result.aabbindex);
			return (true);
		}
	}
	return (false);
}

void	World::_cull_world()
{
	engine->graphic->aabbtree.reset();
	for (unsigned int x = 0; x < size[0]; ++x)
		for (unsigned int y = 0; y < size[1]; ++y)
			for (unsigned int z = 0; z < size[2]; ++z)
				_cull_chunk(chunks[x][y][z], { (float)x * CHUNK_SIZE, (float)y * CHUNK_SIZE, (float)z * CHUNK_SIZE, 0.0f });
}

void	World::_cull_chunk(Chunk &chunk, vec<float, 4> const &position)
{
	for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
		{
			for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
			{
				if (chunk.blocks[x][y][z])/* &&
					(!chunk.blocks[x - 1][y][z] || !chunk.blocks[x + 1][y][z] ||
					!chunk.blocks[x][y - 1][z] || !chunk.blocks[x][y + 1][z] ||
					!chunk.blocks[x][y][z - 1] || !chunk.blocks[x][y][z + 1]))*/
				{
					Aabb	aabb;

					aabb.bottom = { (float)x, (float)y, (float)z, 0.0f };
					aabb.bottom += position;
					aabb.top = aabb.bottom + 1.0f;
					engine->graphic->aabbtree.add_saabb(aabb, chunk.blocks[x][y][z]);
				}
			}
		}
	}
}
