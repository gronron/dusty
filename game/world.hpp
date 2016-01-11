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

#ifndef WORLD_H_
#define WORLD_H_

#include "entity.hpp"
#include "math/vec.hpp"
#include "shape.hpp"

#define CHUNK_SIZE 32

class	World : public Entity
{
	public:

		struct	Chunk
		{
			unsigned char	blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
		};
	
		vec<unsigned int, 4>	size;
		Chunk					***chunks;
		Light	*light;
		double	time;
		
		Axisalignedboxshape	shape;
		Body				*body;

		//int		teamids[2];
		//Team	*teams;
	
		World(Gameengine *, Replication *, int const, short int const, Entity const *);
		virtual ~World();

		//virtual void	postinstanciation();
		//virtual void	destroy();

		//virtual void	get_replication(Packet &) const;
		//virtual void	replicate(Packet &, float);
		
		virtual void	tick(float const);
		
		///////////////////////////////
		
		bool	load(char const *filename);
		bool	save(char const *filename);
		void	fill(vec<int, 4> const &start, vec<int, 4> const &end, char const value);
		bool	create_block(Ray const &ray, char value);
		bool	destroy_block(Ray const &ray);
		
		void	_cull_world();
		void	_cull_chunk(Chunk &chunk, vec<float, 4> const &position);
};

#endif
