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

#include <stdio.h>
#include "selector.hpp"

Selector::Selector()
{

}

Selector::~Selector()
{
	FD_ZERO(&_rfds);
}

bool				Selector::check(float timeout)
{
	struct timeval	tv;
	int				max = 0;
	int				ret;

	tv.tv_sec = (int)timeout;
	tv.tv_usec = (int)((timeout - tv.tv_sec) * 1000000);
	FD_ZERO(&_rfds);
	for (std::list<Socket>::iterator it = _strmlist.begin(); it != _strmlist.end(); it++)
	{
		if (max < *it)
			max = *it;
		FD_SET(*it, &_rfds);
	}
	if ((ret = select(max + 1, &_rfds, 0, 0, timeout < 0.0f ? 0 : &tv)) < 0)
	{
		perror("Error: select()");
		return (false);
	}
	return (ret);
}
