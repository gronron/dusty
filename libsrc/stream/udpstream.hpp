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

#ifndef UDP_STREAM_H_
#define UDP_STREAM_H_

#include <map>
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
		};


		Socket					_id;

		std::map<int, Client>	_cltmap;
		int						_tempid;
		sockaddr_in				_tempaddr;


		Udp_server();
		Udp_server(char const *port, bool ipv6);
		~Udp_server();

		bool		operator()();
		bool		operator()(char const *port, bool ipv6);

		bool		is_good() const;

		char const	*get_clientip(int id) const;
		char const	*get_clientport(int id) const;

		int			addclient();
		void		rmclient(int id);

		int			read(int &id, unsigned int size, void *data);
		int			write(int id, unsigned int size, void const *data);
};

class	Udpstream : public Stream
{
	public:

		static const int	MAXUDPSIZE = 65535;

		Socket	_id;

		Udpstream();
		Udpstream(char const *ip, char const *port, bool ipv6);
		Udpstream(char const *ip, char const *pin, char const *pout, bool ipv6);
		~Udpstream();

		bool	operator()();
		bool	operator()(char const *ip, char const *port, bool ipv6);
		bool	operator()(char const *ip, char const *pin, char const *pout, bool ipv6);

		bool	is_good() const;

		int		read(unsigned int size, void *data);
		int		write(unsigned int size, void const *data);
};

#endif
