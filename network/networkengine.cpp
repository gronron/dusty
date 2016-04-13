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

#include "new.hpp"
#include "time/time.hpp"
#include "client.hpp"
#include "server.hpp"
#include "entity.hpp"
#include "gameengine.hpp"
#include "console.hpp"
#include "networkengine.hpp"

void				*Networkengine::runthrd(void *data)
{
	Networkengine	*ne = (Networkengine *)data;
	double			c;
	double			d;
	double			l;

	if (ne->master)
		ne->srv = new Server(&ne->mq, ne->port.c_str());
	else
		ne->clt = new Client(&ne->mq, ne->ip.c_str(), ne->port.c_str());

	c = dclock();
	while (ne->running)
	{
		l = c;
		d = (c = dclock()) - l;
		dsleep(0.001);
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

Networkengine::Networkengine(Gameengine *g) : engine(g), master(g->master)
{

}

Networkengine::~Networkengine()
{
	running = false;
	delete [] _replications;
	thrd.wait();
}

Replication		*Networkengine::new_replication(int const id)
{
	if (id >= (int)_rsize)
	{
		_replications = resize(_replications, _rsize, _rsize << 1);
		for (unsigned int i = 0; i < _rsize; ++i)
			if (_replications[i].entity)
				_replications[i].entity->rp = _replications + i;
		_rsize <<= 1;
	}
	if (_replications[id].entity)
	{
		//destroy everything
	}
	return (_replications + id);
}

void								Networkengine::tick(float const delta)
{
	Messagequeue::Message			*msg;
	int								id;

	while ((msg = mq.get_in()))
	{
		if (msg->type == Messagequeue::UPDATE)
		{
			id = Replication::get_id(msg->pckt);
			if (id < (int)_rsize && _replications[id].entity) // check type
				_replications[id].replicate(msg->pckt, msg->ping);
			else if (!master)
			{
				Replication	*r = new_replication(id);
				r->init(msg->pckt, msg->ping);
				r->entity = engine->create(r);
				r->entity->replicate(msg->pckt, msg->ping);
			}
		}
		else if (msg->type == Messagequeue::DESTROY)
		{
			id = Replication::get_id(msg->pckt);
			if (!master && id < (int)_rsize && _replications[id].entity)
				_replications[id].destroy();
		}
		else if (msg->type == Messagequeue::CONNECTION)
		{
 			id = engine->create(engine->controllerclass, 0, true)->id;
			_playerids[msg->cltid] = id;
			mq.push_out_cnt(msg->cltid, id);
		}
		else if (msg->type == Messagequeue::DISCONNECTION)
		{
			if (_playerids[msg->cltid] >= 0)
			{
				if (_replications[_playerids[msg->cltid]].entity)
					_replications[_playerids[msg->cltid]].entity->destroy();
				_playerids[msg->cltid] = -1;
			}
		}
		else if (msg->type == Messagequeue::CONTROL)
			engine->control(msg->actid);
		else if (msg->type == Messagequeue::TEXTMSG)
		{
			msg->pckt.write('\0');
			engine->console->put_text(msg->pckt.get_data());
		}
		mq.pop_in();
	}

	for (unsigned int i = 0; i < _rsize; ++i)
	{
		if (_replications[i].entity)
		{
			if (_replications[i].tick(delta))
			{
				if (_replications[i].needupdate && _replications[i].authority > Replication::GLOBAL)
				{
					if (_replications[i].dead)
					{
						mq.push_out_pckt(i);
						_replications[i].lastsendupd = 0.0f;
						_replications[i].needupdate = false;
					}
					else
					{
						Packet	pckt;
						_replications[i].get_replication(pckt);
						mq.push_out_pckt(pckt);
					}
				}
			}
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

void	Networkengine::send_textmsg(char const *str)
{
	mq.push_out_textmsg(str);
}
