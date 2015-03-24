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

#include "networkengine.hpp"
#include "replication.hpp"

int					Replication::get_id(Packet const &pckt)
{
	unsigned int	size;
	char const		*data;

	size = pckt.getsize();
	data = pckt.getdata();
	if (size >= sizeof(short int) + sizeof(int) && data)
		return (*(int *)(data + sizeof(short int)));
	else
		return (0);
}

Replication::Replication(Packet &pckt, float ping) :
	type(0),
	id(0),
	numin(0),
	numout(0),
	updatetime(0.5f),
	timeout(5.0f),
	lastsendupd(0.0f),
	lastrecvupd(0.0f),
	needupdate(true),
	dead(false),
	master(false)
{
	pckt.read(type);
	pckt.read(id);
	pckt.read(numin);
	if (ne->master)
		needupdate = true;
}

Replication::Replication(float updtt, float tmt) :
	type(),
	id(),
	numin(0),
	numout(0),
	updatetime(updtt),
	timeout(tmt),
	lastsendupd(0.0f),
	lastrecvupd(0.0f),
	needupdate(true),
	dead(false),
	master(false)
{
	ne->add(this);
}

Replication::~Replication()
{

}

Packet		*Replication::get_replication()
{
	Packet	*pckt = new Packet();

	lastsendupd = 0.0f;
	needupdate = false;
	pckt->write(type);
	pckt->write(id);
	pckt->write(++numout);
	return (pckt);
}

void			Replication::replicate(Packet &pckt, float p)
{
	short int	t;
	int			i;
	int			n;

	if (!dead)
	{
		pckt.read(t);
		pckt.read(i);
		pckt.read(n);
		if (n > numin)
		{
			lastrecvupd = 0.0f;
			numin = n;
			if (ne->master)
				needupdate = true;
		}
	}
}

bool	Replication::tick(float delta)
{
	if (ne->master || master)
	{
		if ((lastsendupd += delta) > updatetime)
			needupdate = true;
		if (dead && (lastrecvupd += delta) > timeout)
			return (false);
	}
	else
	{
		if ((lastrecvupd += delta) > timeout)
		{
			if (!dead)
				//actor->destroy();
			return (false);
		}
	}
	return (true);
}

void	Replication::destroy()
{
	updatetime = 0.5f;
	timeout = 5.0f;
	lastsendupd = 0.0f;
	lastrecvupd = 0.0f;
	needupdate = true;
	dead = true;
}
