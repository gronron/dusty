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
/*
#include <fstream>
#include "math/vec.hpp"
#include "math/vec_util.hpp"
#include "new.hpp"
#include "aabb.hpp"
#include "chunk.hpp"

Chunk				***load_world(char const *filename, vec<unsigned int, 4> &size)
{
	std::ifstream	is(filename);
	Chunk			***chunks;

	if (is.good())
	{
		is.read((char *)&size, sizeof(vec<float, 4>));
		chunks = new_space<Chunk>(size[0], size[1], size[2]);
		
		unsigned int const	buffer_size = size[0] * size[1] * size[2] * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
		is.read((char *)chunks[0][0], buffer_size);
	}
	return (chunks);
}

bool	save_world(char const *filename, vec<unsigned int, 4> const &size, Chunk const ***chunks)
{
	std::ofstream	os(filename);

	if (os.good())
	{
		os.write((char *)&size, sizeof(vec<float, 4>));

		unsigned int const	buffer_size = size[0] * size[1] * size[2] * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
		os.write((char *)chunks[0][0], buffer_size);
		return (true);
	}
	else
		return (false);
}

void					fill_world(vec<unsigned int, 4> const &size, Chunk ***chunks, vec<int, 4> const &start, vec<int, 4> const &end, char const value)
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
				int const	bi = i / CHUNK_SIZE;
				int const	ci = i % CHUNK_SIZE;
				chunks[ck][cj][ci].blocks[bk][bj][bi] = value;
			}
		}
	}
}

void	generate_chunk(Chunk *chunk)
{
	for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
		{
			int	height = (x * y + x + y) % 7 + 3;
			for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
			{
				chunk->blocks[x][y][z] = z <= height ? 1 : 0; //(x + y) % 2;
			}
		}
	}
}
*/