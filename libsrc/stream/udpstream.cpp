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

#include <cstring>
#include <cstdio>
#include "udpstream.hpp"

Udp_server::Udp_server() : _id(INVALID_SOCKET), _tempid(0)
{

}

Udp_server::Udp_server(char const *port, bool ipv6) : _id(INVALID_SOCKET), _tempid(0)
{
	(*this)(port, ipv6);
}

Udp_server::~Udp_server()
{
	if (_id != INVALID_SOCKET)
		closesocket(_id);
}

bool	Udp_server::operator()()
{
	if (_id != INVALID_SOCKET)
	{
		_tempid = 0;
		_cltmap.clear();
		closesocket(_id);
		_id = INVALID_SOCKET;
	}
	return (true);
}

bool				Udp_server::operator()(char const *port, bool ipv6)
{
	struct addrinfo	*rs;
	struct addrinfo	ai;
	int				err;

	(*this)();
	memset(&ai, 0, sizeof(ai));
	ai.ai_flags = AI_PASSIVE;
	ai.ai_family = ipv6 ? PF_INET6 : PF_INET;
	ai.ai_socktype = SOCK_DGRAM;
	ai.ai_protocol = IPPROTO_UDP;
	if (!(err = getaddrinfo(0, port, &ai, &rs)))
	{
		if ((_id = socket(rs->ai_family, rs->ai_socktype, rs->ai_protocol)) != INVALID_SOCKET)
		{
			if (bind(_id, rs->ai_addr, rs->ai_addrlen))
			{
				perror("Error: bind()");
				(*this)();
			}
		}
		else
			perror("Error: socket()");
		freeaddrinfo(rs);
	}
	else
		fprintf(stderr, "Error: getaddrinfo() %s\n", gai_strerror(err));
	return (_id != INVALID_SOCKET);
}

bool	Udp_server::is_good() const
{
	return (_id != INVALID_SOCKET);
}

char const	*Udp_server::get_clientip(int id) const
{
	std::map<int, Client>::const_iterator	i;

	if ((i = _cltmap.find(id)) != _cltmap.end())
		return (i->second.ip);
	else
		return (0);
}

char const	*Udp_server::get_clientport(int id) const
{
	std::map<int, Client>::const_iterator	i;

	if ((i = _cltmap.find(id)) != _cltmap.end())
		return (i->second.ip);
	else
		return (0);
}

int			Udp_server::add_client()
{
	int		err;
	Client	clt;
	socklen_t sl = sizeof(_tempaddr);

	if (!(err = getnameinfo((struct sockaddr *)&_tempaddr, sl, clt.ip, IP_STRSIZE, clt.port, PORT_STRSIZE, NI_NUMERICHOST | NI_NUMERICSERV)))
	{
		clt.addr = _tempaddr;
		_cltmap[_tempid] = clt;
		return (_tempid++);
	}
	else
	{
		fprintf(stderr, "Error: getnameinfo() %s\n", gai_strerror(err));
		return (-1);
	}
}

void	Udp_server::rm_client(int id)
{
	_cltmap.erase(id);
}

int		Udp_server::read(int &id, unsigned int size, void *data)
{
	std::map<int, Client>::iterator	i;
	int	rsize;
	socklen_t sl = sizeof(_tempaddr);

	id = -1;
	if ((rsize = recvfrom(_id, (char *)data, size, 0, (struct sockaddr *)&_tempaddr, &sl)) >= 0)
	{
		for (i = _cltmap.begin(); i != _cltmap.end(); ++i)
		{
			if (!memcmp(&i->second.addr, &_tempaddr, sl))
			{
				id = i->first;
				break;
			}
		}
	}
	else
		perror("Error: recvfrom()");
	return (rsize);
}

int		Udp_server::write(int id, unsigned int size, void const *data)
{
	std::map<int, Client>::const_iterator	i;
	int	rsize;

	if ((i = _cltmap.find(id)) != _cltmap.end())
	{
		if ((rsize = sendto(_id, (char const *)data, size, 0, (struct sockaddr *)&i->second.addr, sizeof(i->second.addr))) < 0)
			perror("Error: sendto()");
		return (rsize);
	}
	else
		return (-1);
}

///////////////////////////////////////

Udpstream::Udpstream() : _id(INVALID_SOCKET)
{

}

Udpstream::Udpstream(char const *ip, char const *port, bool ipv6) : _id(INVALID_SOCKET)
{
	(*this)(ip, port, ipv6);
}

Udpstream::Udpstream(char const *ip, char const *pin, char const *pout, bool ipv6) : _id(INVALID_SOCKET)
{
	(*this)(ip, pin, pout, ipv6);
}

Udpstream::~Udpstream()
{
	if (_id != INVALID_SOCKET)
		closesocket(_id);
}

bool	Udpstream::operator()()
{
	if (_id != INVALID_SOCKET)
	{
		closesocket(_id);
		_id = INVALID_SOCKET;
	}
	return (true);
}

bool				Udpstream::operator()(char const *ip, char const *port, bool ipv6)
{
	struct addrinfo	*rs;
	struct addrinfo	ai;
	int				err;

	(*this)();
	memset(&ai, 0, sizeof(ai));
	ai.ai_flags = AI_PASSIVE;
	ai.ai_family = ipv6 ? PF_INET6 : PF_INET;
	ai.ai_socktype = SOCK_DGRAM;
	if (!(err = getaddrinfo(ip, port, &ai, &rs)))
	{
		if ((_id = socket(rs->ai_family, rs->ai_socktype, rs->ai_protocol)) != INVALID_SOCKET)
		{
			if (connect(_id, rs->ai_addr, rs->ai_addrlen))
			{
				perror("Error: connect()");
				(*this)();
			}
		}
		else
			perror("Error: socket()");
	}
	else
		fprintf(stderr, "Error: getaddrinfo() %s\n", gai_strerror(err));
	freeaddrinfo(rs);
	return (_id != INVALID_SOCKET);
}

bool				Udpstream::operator()(char const *ip, char const *pin, char const *pout, bool ipv6)
{
	struct addrinfo	*rs, *rb;
	struct addrinfo	ai;
	int				err;

	(*this)();
	memset(&ai, 0, sizeof(ai));
	ai.ai_flags = AI_PASSIVE;
	ai.ai_family = ipv6 ? PF_INET6 : PF_INET;
	ai.ai_socktype = SOCK_DGRAM;
	if (!(err = getaddrinfo(ip, pout, &ai, &rs)))
	{
		if (!(err = getaddrinfo(0, pin, &ai, &rb)))
		{
			if ((_id = socket(rs->ai_family, rs->ai_socktype, rs->ai_protocol)) != INVALID_SOCKET)
			{
				if (bind(_id, rb->ai_addr, rb->ai_addrlen))
				{
					perror("Error: bind()");
					(*this)();
				}
				else if (connect(_id, rs->ai_addr, rs->ai_addrlen))
				{
					perror("Error: connect()");
					(*this)();
				}
			}
			else
				perror("Error: socket()");
			freeaddrinfo(rb);
		}
		else
			fprintf(stderr, "Error: getaddrinfo() %s\n", gai_strerror(err));
		freeaddrinfo(rs);
	}
	else
		fprintf(stderr, "Error: getaddrinfo() %s\n", gai_strerror(err));
	return (_id != INVALID_SOCKET);
}

bool	Udpstream::is_good() const
{
	return (_id != INVALID_SOCKET);
}

int			Udpstream::read(unsigned int size, void *data)
{
	int		rsize;

	if (_id != INVALID_SOCKET)
	{
		if ((rsize = recv(_id, (char *)data, size, 0)) < 0)
			perror("Error: recv()");
		return (rsize);
	}
	else
		return (-1);
}

int			Udpstream::write(unsigned int size, void const *data)
{
	int	rsize;

	if (_id != INVALID_SOCKET)
	{
		if ((rsize = send(_id, (char const *)data, size, 0)) < 0)
			perror("Error: recv()");
		return (rsize);
	}
	else
		return (-1);
}
