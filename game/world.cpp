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

#include "common.hpp"
#include "world.hpp"
#include <iostream>

FACTORYREG(World);

World::World(Gameengine *g, Replication *r, int const i, short int const t, Actor const *o) : Actor(g, r, i, t, o)
{
	chunks = new Chunk;
	generate_chunk(chunks);
	
	if (engine->graphic)
	{
		engine->graphic->new_light(&light);
		light->position = 0.0f;
		light->color = 1.0f;
		light->power = 1000.0f;
		
		engine->graphic->materials_count = 3;
		engine->graphic->materials[0].color = { 0.0f, 1.0f, 1.0f };
		engine->graphic->materials[0].transparency = 1.0f;
		engine->graphic->materials[1].color = { 1.0f, 0.8f, 0.2f };
		engine->graphic->materials[1].transparency = 1.0f;
		engine->graphic->materials[2].color = { 1.0f, 0.0f, 1.0f };
		engine->graphic->materials[2].transparency = 1.0f;

		for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
			{
				for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
				{
					//if (chunks->blocks[x][y][z])
					{
						Aabb	aabb;

						aabb.bottom = { (float)x, (float)y, (float)z, 0.0f };
						aabb.top = aabb.bottom + 1.0f;
						engine->graphic->aabbtree.add_saabb(aabb, chunks->blocks[x][y][z]);
					}
				}
			}
		}
	}
}

World::~World()
{
	delete chunks;
}

void	World::tick(float const delta)
{
	time += delta;
	
	time = 1.0f;
	light->position[1] = sin(time) * delta * 40000.0f;
	light->position[2] = cos(time) * delta * 40000.0f;
}