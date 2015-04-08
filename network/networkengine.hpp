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
		
		unsigned int	_rsize;
		Replication		*_replications;

		std::map<int, int>				_playeridmap;

		Messagequeue	mq;

		Client	*clt;
		Server	*srv;
		Thread	thrd;
		bool	running;


		Networkengine(Actormanager *);
		~Networkengine();

		Replication		*new_replication(int const id);
		
		void	tick(float);

		void	connect(std::string const &, std::string const &);

		void	sendtextmsg(std::string const &);
};

#endif
