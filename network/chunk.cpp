/******************************************************************************
Copyright (c) 2015-2016, Geoffrey TOURON
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

#include "rudpchunk.hpp"

Chunksender::Chunksender()
{
	//alloc
}

Chunksender::~Chunksender()
{
	//desalloc
}

void	Chunksender::acknowledge(int const cltid, unsigned int const size, Udpchunkack const *ack)
{
	//find chunk
	//find client
	//ack
}

void	Chunksender::send(Message *msg)
{
	//allocate and create
}

void	Chunksender::tick(float const, Udpserver &)
{
	//
}

void	data_to_chunk(unsigned int const size, char const *data, Chunk &chnk)
{
	unsigned int	csize = LZ4_compressBound(size);
	char 			*cdata = new char[csize];

	csize = LZ4_compress_default(data, cdata, (int)size, (int)csize);

	chnk.blckcnt = csize / BLOCKSIZE + (csize % BLOCKSIZE ? 1 : 0);
	chnk.blocks = new Block[chnk.blckcnt];

	for (unsigned int i = 0; i < chnk.blckcnt; ++i)
	{
		chnk.blocks[i].chnkid = chnk.id;
		chnk.blocks[i].blckcnt = chnk.blckcnt;
		chnk.blocks[i].blckid = i;
		chnk.blocks[i].size = BLOCKSIZE;
		chnk.blocks[i].size = i == chnk.blckcnt - 1 && csize % BLOCKSIZE ? csize % BLOCKSIZE : BLOCKSIZE;
		memcpy(data + i * BLOCKSIZE, chnk.blocks[i].data, chnk.blocks[i].size);
	}
	
	delete [] cdata;
}

void	chunk_to_data(Chunk &chnk, unsigned int &size, char *data)
{
	unsigned int csize = (chnk.blckcnt - 1) * BLOCKSIZE + chnk.blocks[chnk.blckcnt - 1].size;
	char	*cdata = new char[csize];
	
	for (unsigned int i = 0; i < chnk.blckcnt; ++i)
		memcpy(chnk.blocks[i].data, cdata + i * BLOCKSIZE, chnk.blocks[i].size);
	
	size = LZ4_decompress_safe(cdata, data, csize, size);
	
	delete [] cdata;
}