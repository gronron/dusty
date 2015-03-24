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

#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "stream/selector.hpp"
#include "stream/tcpstream.hpp"
#include "stream/udpstream.hpp"
#include "networkproto.hpp"

class	Messagequeue;

class	Server
{
	public:

		struct	Client
		{
			int			id;
			char		ip[IP_STRSIZE];
			char		port[PORT_STRSIZE];
			Tcpstream	*tcp;
			int			udpid;
			Ping		*ping;
		};


		static float const	pingrate;
		static float const	timeout;


		Messagequeue		*mq;

		Selector			_slctr;
		Tcp_server			_tcpsrv;
		Udp_server			_udpsrv;
		std::list<Client>	_cltlist;
		int					_currentid;

		Server(Messagequeue *, std::string const &);
		~Server();

		void	tick(float);

		void	_addclient();
		void	_pingclient(float);
		bool	_comtcpclient(std::list<Client>::iterator &);
		void	_receivepacket();
		void	_sendpacket();
		void	_delclient(std::list<Client>::iterator	&);
};

#endif
