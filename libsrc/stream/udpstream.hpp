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

#ifndef UDP_STREAM_H_
#define UDP_STREAM_H_

#include "socket.hpp"
#include "stream.hpp"

class	Udp_server
{
	public:

		struct	Client
		{
			sockaddr_in	addr;
			char		ip[IP_STRSIZE];
			char		port[PORT_STRSIZE];
			int			next;
		};


		Socket			_id;

		unsigned int	_maxclts;
		Client			*_clients;
		int				_free;
		
		sockaddr_in		_tempaddr;


		Udp_server();
		Udp_server(char const *port, bool const ipv6);
		~Udp_server();

		bool		operator()();
		bool		operator()(char const *port, bool const ipv6);

		bool		is_good() const;

		char const	*get_clientip(int const id) const;
		char const	*get_clientport(int const id) const;

		int			add_client();
		void		rm_client(int const id);

		int			read(int &id, unsigned int const size, void *data);
		int			write(int const id, unsigned int const size, void const *data);
};

class	Udpstream : public Stream
{
	public:

		static int const	MAXUDPSIZE = 65535;

		Socket	_id;

		Udpstream();
		Udpstream(char const *ip, char const *port, bool ipv6);
		Udpstream(char const *ip, char const *pin, char const *pout, bool ipv6);
		~Udpstream();

		bool	operator()();
		bool	operator()(char const *ip, char const *port, bool ipv6);
		bool	operator()(char const *ip, char const *pin, char const *pout, bool ipv6);

		bool	is_good() const;

		int		read(unsigned int const size, void *data);
		int		write(unsigned int const size, void const *data);
};

#endif
