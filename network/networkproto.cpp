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

#include <cstring>
#include "networkproto.hpp"

Ping::Ping(int const sn) : ping(0.0f), timer(0.0f), splnbr(sn), idx(0), tempping(new float[sn]), pinging(false)
{
	memset(tempping, 0, splnbr * sizeof(float));
}

Ping::~Ping()
{
	delete [] tempping;
}

void	Ping::start()
{
	pinging = true;
	timer = 0.0f;
	tempping[idx] = 0.0f;
}

void	Ping::stop()
{
	pinging = false;
	for (int i = 0; i < splnbr; ++i)
		ping = tempping[i];
	ping /= splnbr;
	++idx;
	idx %= splnbr;
}
