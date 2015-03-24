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

#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include "stream/selector.hpp"
#include "stream/tcpstream.hpp"
#include "stream/udpstream.hpp"
#include "networkproto.hpp"

class	Messagequeue;

class	Client
{
	public:

		static float const	timeout;


		Messagequeue	*mq;

		Selector		_slctr;
		Tcpstream		_tcp;
		Udpstream		_udp;
		int				cntid;
		Ping			ping;
		bool			connected;
		float			timer;


		Client(Messagequeue *, std::string const &ip, std::string const &port);
		~Client();

		void	tick(float);

		void	_comtcp();
};

#endif
