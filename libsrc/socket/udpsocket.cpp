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
#include "../new.hpp"
#include "udpsocket.hpp"

UDPServer::UDPServer() : _id(INVALID_SOCKET), _maxclts(64), _clients(0), _free(0)
{
	_clients = new Client[_maxclts];
	for (unsigned int i = 0; i < _maxclts; ++i)
		_clients[i].next = i + 1;
	_clients[_maxclts - 1].next = -1;
}

UDPServer::UDPServer(char const *port, bool const ipv6) : _id(INVALID_SOCKET), _maxclts(64), _clients(0), _free(0)
{
	_clients = new Client[_maxclts];
	for (unsigned int i = 0; i < _maxclts; ++i)
		_clients[i].next = i + 1;
	_clients[_maxclts - 1].next = -1;
	(*this)(port, ipv6);
}

UDPServer::~UDPServer()
{
	if (_id != INVALID_SOCKET)
		closesocket(_id);
	delete [] _clients;
}

bool	UDPServer::operator()()
{
	if (_id != INVALID_SOCKET)
	{
		for (unsigned int i = 0; i < _maxclts; ++i)
			_clients[i].next = i + 1;
		_clients[_maxclts - 1].next = -1;
		_free = 0;
		closesocket(_id);
		_id = INVALID_SOCKET;
	}
	return (true);
}

bool				UDPServer::operator()(char const *port, bool const ipv6)
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
				p_perror("error! bind()");
				(*this)();
			}
		}
		else
			p_perror("error! socket()");
		freeaddrinfo(rs);
	}
	else
		fprintf(stderr, "error! getaddrinfo() %s\n", gai_strerror(err));
	return (_id != INVALID_SOCKET);
}

bool	UDPServer::is_good() const
{
	return (_id != INVALID_SOCKET);
}

char const	*UDPServer::get_clientip(int const id) const
{
	if (id >= 0 && _clients[id].next < 0)
		return (_clients[id].ip);
	else
		return (0);
}

char const	*UDPServer::get_clientport(int const id) const
{
	if (id >= 0 && _clients[id].next < 0)
		return (_clients[id].port);
	else
		return (0);
}

int					UDPServer::add_client()
{
	unsigned int	id;
	int				err;

	if (_free < 0)
	{
		_free = _maxclts;
		_maxclts <<= 1;
		_clients = resize(_clients, _free, _maxclts);
		for (unsigned int i = _free; i < _maxclts; ++i)
			_clients[i].next = i + 1;
		_clients[_maxclts - 1].next = -1;
	}
	id = _free;
	_free = _clients[_free].next;
	if (!(err = getnameinfo((struct sockaddr *)&_tempaddr, sizeof(_tempaddr), _clients[id].ip, IP_STRSIZE, _clients[id].port, PORT_STRSIZE, NI_NUMERICHOST | NI_NUMERICSERV)))
	{
		_clients[id].addr = _tempaddr;
		_clients[id].next = -1;
		return (id);
	}
	else
	{
		fprintf(stderr, "error! getnameinfo() %s\n", gai_strerror(err));
		return (-1);
	}
}

void	UDPServer::rm_client(int const id)
{
	if (id >= 0 && _clients[id].next < 0)
	{
		 _clients[id].next = _free;
		_free = id;
	}
}

int				UDPServer::read(int &id, unsigned int const size, void *data)
{
	socklen_t	sl = sizeof(_tempaddr);
	int			rsize;

	id = -1;
	if ((rsize = recvfrom(_id, (char *)data, size, 0, (struct sockaddr *)&_tempaddr, &sl)) >= 0)
	{
		for (unsigned int i = 0; i < _maxclts; ++i)
		{
			if (_clients[i].next < 0 && !memcmp(&_clients[i].addr, &_tempaddr, sl))
			{
				id = i;
				break;
			}
		}
	}
	else
		p_perror("error! recvfrom()");
	return (rsize);
}

int		UDPServer::write(int const id, unsigned int const size, void const *data)
{
	int	rsize;

	if (id >= 0 && _clients[id].next < 0)
	{
		if ((rsize = sendto(_id, (char const *)data, size, 0, (struct sockaddr *)&_clients[id].addr, sizeof(_clients[id].addr))) < 0)
			p_perror("error! sendto()");
		return (rsize);
	}
	else
		return (-1);
}

///////////////////////////////////////

UDPSocket::UDPSocket() : _id(INVALID_SOCKET)
{

}

UDPSocket::UDPSocket(char const *ip, char const *port, bool const ipv6) : _id(INVALID_SOCKET)
{
	(*this)(ip, port, ipv6);
}

UDPSocket::UDPSocket(char const *ip, char const *pin, char const *pout, bool const ipv6) : _id(INVALID_SOCKET)
{
	(*this)(ip, pin, pout, ipv6);
}

UDPSocket::~UDPSocket()
{
	if (_id != INVALID_SOCKET)
		closesocket(_id);
}

bool	UDPSocket::operator()()
{
	if (_id != INVALID_SOCKET)
	{
		closesocket(_id);
		_id = INVALID_SOCKET;
	}
	return (true);
}

bool				UDPSocket::operator()(char const *ip, char const *port, bool const ipv6)
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
				p_perror("error! connect()");
				(*this)();
			}
		}
		else
			p_perror("error! socket()");
	}
	else
		fprintf(stderr, "error! getaddrinfo() %s\n", gai_strerror(err));
	freeaddrinfo(rs);
	return (_id != INVALID_SOCKET);
}

bool				UDPSocket::operator()(char const *ip, char const *pin, char const *pout, bool const ipv6)
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
					p_perror("error! bind()");
					(*this)();
				}
				else if (connect(_id, rs->ai_addr, rs->ai_addrlen))
				{
					p_perror("error! connect()");
					(*this)();
				}
			}
			else
				p_perror("error! socket()");
			freeaddrinfo(rb);
		}
		else
			fprintf(stderr, "error! getaddrinfo() %s\n", gai_strerror(err));
		freeaddrinfo(rs);
	}
	else
		fprintf(stderr, "error! getaddrinfo() %s\n", gai_strerror(err));
	return (_id != INVALID_SOCKET);
}

bool	UDPSocket::is_good() const
{
	return (_id != INVALID_SOCKET);
}

int			UDPSocket::read(unsigned int const size, void *data)
{
	int		rsize;

	if (_id != INVALID_SOCKET)
	{
		if ((rsize = recv(_id, (char *)data, size, 0)) < 0)
			p_perror("error! recv()");
		return (rsize);
	}
	else
		return (-1);
}

int			UDPSocket::write(unsigned int const size, void const *data)
{
	int	rsize;

	if (_id != INVALID_SOCKET)
	{
		if ((rsize = send(_id, (char const *)data, size, 0)) < 0)
			p_perror("error! recv()");
		return (rsize);
	}
	else
		return (-1);
}
