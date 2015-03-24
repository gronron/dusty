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

#ifndef SOCKET_H_
#define SOCKET_H_

#if defined(_WIN32) || defined(__WIN32__)
	#include <SDKDDKVer.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>

	typedef	SOCKET	Socket;
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>

	#define INVALID_SOCKET	-1
	#define closesocket(id)	close(id)

	typedef int		Socket;
#endif

int const	IP_STRSIZE = 64;
int const	PORT_STRSIZE = 8;

#endif
