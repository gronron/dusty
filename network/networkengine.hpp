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

#ifndef NETWORKENGINE_H_
#define NETWORKENGINE_H_

#include <string>
#include <map>
#include "replication.hpp"
#include "messagequeue.hpp"
#include "thread/thread.hpp"

class	Actormanager;
class	Client;
class	Server;

class	Networkengine
{
	public:

		static	void	*runthrd(void *);


		Actormanager	*am;

		bool	master;
		std::string	ip;
		std::string	port;

		std::map<int, Replication *>	_replicationmap;
		std::map<int, int>				_playeridmap;

		Messagequeue	mq;

		Client	*clt;
		Server	*srv;
		Thread	thrd;
		bool	running;


		Networkengine(Actormanager *);
		~Networkengine();

		void	add(Replication *);

		void	tick(float);

		void	connect(std::string const &, std::string const &);

		void	sendtextmsg(std::string const &);
};

#endif
