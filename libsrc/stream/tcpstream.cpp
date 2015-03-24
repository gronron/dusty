///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2012 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of Chasis.
//
//    Chasis is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Chasis is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Chasis.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

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
