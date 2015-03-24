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
