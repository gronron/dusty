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

#include "endian/packet.hpp"
#include "body.hpp"

Body::Body(Actor *a, bool d, bool s) : actor(a), dynamic(d), solid(s), ping(0.0f)
{
	mass = 0.0f;
	loc = 0.0f;
	spd = 0.0f;
	acc = 0.0f;
	size = 0.0f;
}

Body::~Body()
{

}

void	Body::get_replication(Packet &pckt)
{
	pckt.write(mass);
	pckt.write(loc);
	pckt.write(spd);
	pckt.write(acc);
	pckt.write(size);
}

void	Body::replicate(Packet &pckt, float p)
{
	ping = p;
	pckt.read(mass);
	pckt.read(loc);
	pckt.read(spd);
	pckt.read(acc);
	pckt.read(size);
}
