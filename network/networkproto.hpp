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

#ifndef NETPROTO_H_
#define NETPROTO_H_

#define FIELDSIZE 64
#define PACKETBUFFER 512
/*
enum	Prototype
{
	NONE = 0,
	OUDP,
	RUDP,
	ACKRUDP,
	CHUNK,
	ACKCHUNK
};

struct					Header
{
	unsigned char		type;
	unsigned char		channel;
	short unsigned int	size;
};

struct					Rudp
{
	Header				hdr;
	unsigned short int	id;
	unsigned char		data[1024];
};

struct					Rudpack
{
	Header				hdr;
	unsigned short int	ackid;
	unsigned char		ackfield[FIELDSIZE];
};

struct				Udpchunk
{
	Header			hdr;
	unsigned int	chunkid;
	unsigned int	chunksize;
	unsigned int	blockid;
	unsigned char	data[1024];
};

struct				Udpchunkack
{
	Header			hdr;
	unsigned int	chunkid;
	unsigned int	ackid;
	unsigned char	ackfield[FIELDSIZE];
};
*/

struct	Header
{
	short int		size;
	unsigned char	type;
};

enum	Prototype
{
	CNTID,
	FPING,
	SPING,
	TPING,
	MID,
	TEXTMSG
};

class	Ping
{
	public:

		float	ping;
		float	timer;
		int		splnbr;
		int		idx;
		float	*tempping;
		bool	pinging;

		Ping(int const sn);
		~Ping();

		void	start();
		void	stop();
};

#endif
