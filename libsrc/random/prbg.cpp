///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2014 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of libsrc.
//
//    libsrc is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsrc is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsrc.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#include "mt19937.hpp"
#include "prbg.hpp"

Prbg::Prbg(float c)
{
	if (c > 0.5f)
	{
		chance = 1.0f - c;
		inverter = true;
	}
	else
	{
		chance = c;
		inverter = false;
	}
	a = MT().genrand_real1(0, 2.0 - chance);
}

Prbg::~Prbg()
{

}

void	Prbg::operator()(float c)
{
	if (c > 0.5f)
	{
		chance = 1.0f - c;
		inverter = true;
	}
	else
	{
		chance = c;
		inverter = false;
	}
	a = MT().genrand_real1(0, 2.0 - chance);
}

bool	Prbg::pick()
{
	if ((counter -= chance) <= 0.0f)
	{
		counter = MT().genrand_real1(0, 2.0 - chance);
		return (!inverter);
	}
	else
		return (inverter);
}
