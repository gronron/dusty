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

#include "team.hpp"

void	Team::join(int const idx)
{
	if (playercount < 64)
		playerid[playercount++] = idx;
}

void	Team::leave(int const idx)
{
	for (unsigned int i = 0; i < 64; ++i)
	{
		if (playerird[i] == idx)
		{
			playerid[i] = playerid[--playercount];
			break;
		}
	}
}

void	Team::push_to_spawnqueue(int const idx)
{
	if ((sqfront + 1) % 64 != sqback)
	{
		spawnqueue[sqfront] = idx;
		sqfront = (sqfront + 1) % 64;
	}
}

int		Team::pop_from_spawnqueue()
{
	int	idx = -1;
	
	if (sqfront != sqback)
	{
		idx = spawnqueue[sqback];
		sqback = (sqback + 1) % 64;
	}
	return (idx);
}

bool	Team::create_structure(unsigned int const hash, vec<float, 4> const &position)
{
	//check reactor
	//cheack build point
	//create it
	//init it
}

bool	Team::destroy_structure(int const idx)
{
	for (unsigned int i = 0; i < structcount; ++i)
	{
		if (structureid[i] == idx)
		{
			Structure *strct = (Structure *)engine->find(idx);
			
			if (strct->status != Entity::DESTROYED)
				strct->destroy();
			buildpoint += strct->buildpoint;
			structureid[i] = structureid[--structcount];
		}
	}
}
