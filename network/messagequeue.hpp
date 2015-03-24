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

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <string>
#include "endian/packet.hpp"

class	Messagequeue
{
	public:

		enum		Target { GLBROADCAST, LCBROADCAST, GLTEAM, LCTEAM, PLAYERS };
		enum		Type { CONNECTION, DISCONNECTION, UPDATE, DESTROY, CONTROL, TEXTMSG };

		struct		Message
		{
			char	target;
			char	type;
			int		cltid;
			int		actid;
			float	ping;
			Packet	*pckt;
		};


		unsigned int	_size;
		unsigned int	_frontin;
		unsigned int	_frontout;
		unsigned int	_backin;
		unsigned int	_backout;
		Message			*_msgin;
		Message			*_msgout;


		Messagequeue();
		~Messagequeue();

		void	push_in_pckt(int cltid, float ping, int size, char *data);
		void	push_in_cnt(int cltid);
		void	push_in_discnt(int cltid);
		void	push_in_cntrl(int actid);
		void	push_in_textmsg(int size, char *data);
		void	push_in_textmsg(int cltid, int size, char *data);

		void	push_out_pckt(Packet *pckt);
		void	push_out_pckt(int actorid);
		void	push_out_cnt(int cltid, int actid);
		void	push_out_discnt(int cltid);
		void	push_out_textmsg(std::string const &msg);
		void	push_out_textmsg(int cltid, std::string const &msg);

		Message	*get_in();
		void	pop_in();
		Message	*get_out();
		void	pop_out();
};

#endif
