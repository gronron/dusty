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

#include "time/time.hpp"
#include "client.hpp"
#include "server.hpp"
#include "actor.hpp"
#include "actormanager.hpp"
#include "console.hpp"
#include "networkengine.hpp"

void				*Networkengine::runthrd(void *data)
{
	Networkengine	*ne = (Networkengine *)data;
	double			c;
	double			d;
	double			l;

	if (ne->master)
		ne->srv = new Server(&ne->mq, ne->port);
	else
		ne->clt = new Client(&ne->mq, ne->ip, ne->port);

	c = src::clock();
	while (ne->running)
	{
		l = c;
		d = (c = src::clock()) - l;
		src::sleep(0.001);
		if (ne->master)
			ne->srv->tick((float)d);
		else
			ne->clt->tick((float)d);
	}
	if (ne->master)
		delete ne->srv;
	else
		delete ne->clt;
	return (0);
}

Networkengine::Networkengine(Actormanager *a) : am(a), master(a->master)
{

}

Networkengine::~Networkengine()
{
	std::map<int, Replication *>::iterator	i;

	running = false;
	for (i = _replicationmap.begin(); i != _replicationmap.end(); ++i)
		delete i->second;
	thrd.wait();
}

void	Networkengine::add(Replication *ri)
{
	_replicationmap[ri->id] = ri;
}

void										Networkengine::tick(float delta)
{
	std::map<int, Replication *>::iterator	i;
	std::map<int, int>::iterator			j;
	Messagequeue::Message					*msg;
	int										id;

	while ((msg = mq.get_in()))
	{
		if (msg->type == Messagequeue::UPDATE)
		{
			id = Replication::get_id(*msg->pckt);
			if ((i = _replicationmap.find(id)) != _replicationmap.end())
				i->second->replicate(*msg->pckt, msg->ping);
			else if (!master)
				_replicationmap[id] = new Replication(am, *msg->pckt, msg->ping);
			delete msg->pckt;
		}
		else if (msg->type == Messagequeue::DESTROY)
		{
			if (!master && (i = _replicationmap.find(Replication::get_id(*msg->pckt))) != _replicationmap.end())
				i->second->actor->destroy();
			delete msg->pckt;
		}
		else if (msg->type == Messagequeue::CONNECTION)
		{
 			id = am->create(am->controllerclass, 0)->id;
			_playeridmap[msg->cltid] = id;
			mq.push_out_cnt(msg->cltid, id);
		}
		else if (msg->type == Messagequeue::DISCONNECTION)
		{
			if ((j = _playeridmap.find(msg->cltid)) != _playeridmap.end())
				if ((i = _replicationmap.find(j->second)) != _replicationmap.end())
					i->second->actor->destroy();
		}
		else if (msg->type == Messagequeue::CONTROL)
			am->control(msg->actid);
		else if (msg->type == Messagequeue::TEXTMSG)
			am->cl->puttext(std::string(msg->pckt->getdata(), msg->pckt->getsize()));
		mq->pop_in();
	}

	for (i = _replicationmap.begin(); i != _replicationmap.end();)
	{
		if (!i->second->tick(delta))
		{
			delete i->second;
			_replicationmap.erase(i++);
		}
		else
		{
			if (i->second->needupdate && (master || i->second->master))
			{
				if (i->second->dead)
				{
					mq.push_out_pckt(i->second->id);
					i->second->lastsendupd = 0.0f;
					i->second->needupdate = false;
				}
				else
					mq.push_out_pckt(i->second->get_replication());
			}
			++i;
		}
	}
}

void	Networkengine::connect(std::string const &i, std::string const &p)
{
	ip = i;
	port = p;
	running = true;
	thrd.create(Networkengine::runthrd, this);
}

void	Networkengine::sendtextmsg(std::string const &str)
{
	mq.push_out_textmsg(str);
}
