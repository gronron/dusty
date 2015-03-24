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

#ifndef TCP_STREAM_H_
#define TCP_STREAM_H_

#include "socket.hpp"
#include "stream.hpp"

class	Tcp_server
{
	public:

		Socket	_id;


		Tcp_server();
		Tcp_server(char const *port, bool ipv6);
		~Tcp_server();

		bool	operator()();
		bool	operator()(char const *port, bool ipv6);

		bool	is_good() const;
};

class	Tcpstream : public Stream
{
	public:

		Socket	_id;


		Tcpstream();
		Tcpstream(Tcp_server &srv, char *ip, char *port);
		Tcpstream(char const *ip, char const *port);
		~Tcpstream();

		bool	operator()();
		bool	operator()(Tcp_server &, char *ip, char *port);
		bool	operator()(char const *ip, char const *port);

		bool	is_good() const;

		int		read(unsigned int size, void *data);
		int		write(unsigned int size, void const *data);
};

#endif
