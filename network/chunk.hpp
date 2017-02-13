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

#ifndef RUDPCHUNK_H_
#define RUDPCHUNK_H_

#define BLOCKSIZE 1024

class	Chunksender
{
	public:
	
		struct	Client
		{
			unsigned int	bucket;
			unsigned int	front;
			unsigned int	back;
			unsigned int	chunkqueue[256];
			Ack				acks[8];
		};
	
		struct				Ack
		{
			unsigned int	id;
			unsigned char	field[64];
		}
	
		struct				Chunk
		{
			unsigned int	id;
			unsigned int	blockcount;
			Udpchunk		*blocks;
			unsigned int	clientcount;
			Ack				*clientacks;
		};
	
		unsigned int	count;
		unsigned int	size;
		Chunk			*chunks;


		Chunksender();
		~Chunksender();
	
		void	acknowledge(int const, unsigned int const size, Udpchunkack const *);
		void	send(Message *);
		void	tick(float const, Udpserver &);
};

class	Chunkreicever
{
	public:
	
		struct				Chunk
		{
			unsigned int	id;
			unsigned int	size;
			unsigned char	*data;
			unsigned int	ackid;
			unsigned char	*ackfield;
		};
	
		unsigned int	count;
		unsigned int	size;
		Chunk			*chunks;
		
		unsigned int	activeid;
		unsigned char	activefield[64];
	
		void	check(unsigned int const size, Udpchunk const*);
		bool	tick(float const, Udpsocket &);
};

#endif
