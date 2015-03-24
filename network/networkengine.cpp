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
