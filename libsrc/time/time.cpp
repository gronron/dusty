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

#if defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include "time.hpp"

namespace	src
{

	#if defined(_WIN32) || defined(__WIN32__)
	void				sleep(double const second)
	{
		Sleep((DWORD)(second * 1000.0));
	}

	double				clock()
	{
		LARGE_INTEGER	a, b;

		QueryPerformanceFrequency(&a);
		QueryPerformanceCounter(&b);
		return ((double)b.QuadPart / a.QuadPart);
	}

	#else

	void				sleep(double const second)
	{
		struct timespec	a;

		a.tv_sec = (int)second;
		a.tv_nsec = (int)((second - a.tv_sec) * 1000000000.0);
		clock_nanosleep(CLOCK_REALTIME, 0, &a, 0);
	}

	double				clock()
	{
		struct timespec	a;

		clock_gettime(CLOCK_REALTIME, &a); // CLOCK_MONOTONIC
		return (a.tv_sec + a.tv_nsec / 1000000000.0);
	}
	#endif

}
