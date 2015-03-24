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
#include "tcpstream.hpp"

Tcp_server::Tcp_server() : _id(INVALID_SOCKET)
{

}

Tcp_server::Tcp_server(char const *port, bool ipv6) : _id(INVALID_SOCKET)
{
	(*this)(port, ipv6);
}

Tcp_server::~Tcp_server()
{
	if (_id != INVALID_SOCKET)
		closesocket(_id);
}

bool	Tcp_server::operator()()
{
	if (_id != INVALID_SOCKET)
	{
		closesocket(_id);
		_id = INVALID_SOCKET;
	}
	return (true);
}

bool				Tcp_server::operator()(char const *port, bool ipv6)
{
	struct addrinfo	*res;
	struct addrinfo	hints;
	int				err;

	(*this)();
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = ipv6 ? PF_INET6 : PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (!(err = getaddrinfo(0, port, &hints, &res)))
	{
		if ((_id = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) != INVALID_SOCKET)
		{
			if (bind(_id, res->ai_addr, res->ai_addrlen))
			{
				perror("Error: bind()");
				(*this)();
			}
			else if (listen(_id, 5))
			{
				perror("Error: listen()");
				(*this)();
			}
		}
		else
			perror("Error: socket()");
		freeaddrinfo(res);
	}
	else
		fprintf(stderr, "Error: getaddrinfo() %s\n", gai_strerror(err));
	return (_id != INVALID_SOCKET);
}

bool	Tcp_server::is_good() const
{
	return (_id != INVALID_SOCKET);
}

///////////////////////////////////////

Tcpstream::Tcpstream() : _id(INVALID_SOCKET)
{

}

Tcpstream::Tcpstream(Tcp_server &x, char *ip, char *port) : _id(INVALID_SOCKET)
{
	(*this)(x, ip, port);
}

Tcpstream::Tcpstream(char const *ip, char const *port) : _id(INVALID_SOCKET)
{
	(*this)(ip, port);
}

Tcpstream::~Tcpstream()
{
	if (_id != INVALID_SOCKET)
		closesocket(_id);
}

bool	Tcpstream::operator()()
{
	if (_id != INVALID_SOCKET)
	{
		closesocket(_id);
		_id = INVALID_SOCKET;
	}
	return (true);
}

bool	Tcpstream::operator()(Tcp_server &x, char *ip, char *port)
{
	(*this)();
	if (x._id != INVALID_SOCKET)
	{
		struct sockaddr_in	b;
		socklen_t c = sizeof(b);
		if ((_id = accept(x._id, (struct sockaddr *)&b, &c)) == INVALID_SOCKET)
			perror("Error: accept()");
		else if (ip || port)
		{
			int	err;
			if ((err = getnameinfo((struct sockaddr *)&b, c, ip, ip ? IP_STRSIZE : 0, port, port ? PORT_STRSIZE : 0, NI_NUMERICHOST | NI_NUMERICSERV)))
				fprintf(stderr, "Error: getnameinfo() %s\n", gai_strerror(err));
		}
	}
	return (_id != INVALID_SOCKET);
}

bool				Tcpstream::operator()(char const *ip, char const *port)
{
	struct addrinfo	*res;
	struct addrinfo	hints;
	int				err;

	(*this)();
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = PF_UNSPEC/*PF_INET PF_INET6*/;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (!(err = getaddrinfo(ip, port, &hints, &res)))
	{
		if ((_id = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) != INVALID_SOCKET)
		{
			if (connect(_id, res->ai_addr, res->ai_addrlen))
			{
				perror("Error: connect()");
				(*this)();
			}
		}
		else
			perror("Error: socket()");
		freeaddrinfo(res);
	}
	else
		fprintf(stderr, "Error: getaddrinfo() %s\n", gai_strerror(err));
	return (_id != INVALID_SOCKET);
}

bool	Tcpstream::is_good() const
{
	return (_id != INVALID_SOCKET);
}

int 		Tcpstream::read(unsigned int size, void *data)
{
	int	rsize;

	if (_id != INVALID_SOCKET)
	{
		if ((rsize = recv(_id, (char *)data, size, 0)) < 0)
			perror("Error: recv()");
		return (rsize);
	}
	else
		return (-1);
}

int			Tcpstream::write(unsigned int size, void const *data)
{
	int	rsize;

	if (_id != INVALID_SOCKET)
	{
		if ((rsize = send(_id, (char const *)data, size, 0)) < 0)
			perror("Error: send()");
		return (rsize);
	}
	else
		return (-1);
}
