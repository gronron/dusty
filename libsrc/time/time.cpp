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

#if defined(_WIN32) || defined(__WIN32__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include "time.hpp"

#if defined(_WIN32) || defined(__WIN32__)

void				dsleep(double const second)
{
	Sleep((DWORD)(second * 1000.0));
}

double				dclock()
{
	LARGE_INTEGER	a, b;

	QueryPerformanceFrequency(&a);
	QueryPerformanceCounter(&b);
	return ((double)b.QuadPart / a.QuadPart);
}

#else

void				dsleep(double const second)
{
	struct timespec	a;

	a.tv_sec = (int)second;
	a.tv_nsec = (int)((second - a.tv_sec) * 1000000000.0);
	clock_nanosleep(CLOCK_MONOTONIC, 0, &a, 0);
}

double				dclock()
{
	struct timespec	a;

	clock_gettime(CLOCK_MONOTONIC, &a);
	return (a.tv_sec + a.tv_nsec / 1000000000.0);
}

#endif
