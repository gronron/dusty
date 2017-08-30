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

#include <iostream>
#include "thread.hpp"

#if defined(_WIN32) || defined(__WIN32__)

unsigned int	Thread::get_corenumber()
{
	SYSTEM_INFO	sysinfo;

	GetSystemInfo(&sysinfo);
	return (sysinfo.dwNumberOfProcessors);
}

Thread::Thread() : thr(0), thrid(0)
{

}

Thread::Thread(void *(*function)(void*), void *data) : thr(0), thrid(0)
{
	create(function, data);
}

Thread::~Thread()
{

}

bool	Thread::create(void *(*function)(void*), void *data)
{
	thr = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(function), data, 0, &thrid);
	if (!thr)
	{
		std::cerr << "error! CreateThread()" << std::endl;
		return (false);
	}
	return (true);
}

void	Thread::exit(bool const code)
{
	ExitThread(code ? EXIT_SUCCESS : EXIT_FAILURE);
}

bool	Thread::wait()
{
	return (WaitForSingleObject(thr, INFINITE) == WAIT_OBJECT_0);
}

#else

#ifndef _SYS_PARAM_H
#include <sys/param.h>
#endif

#if defined(BSD)

#include <sys/sysctl.h>

unsigned int	Thread::get_corenumber()
{
	int			cn;
	int			mib[2];
	size_t		len = sizeof(cn);

	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	sysctl(mib, 2, &cn, &len, 0, 0);
	return (cn);
}

#else

#include <unistd.h>

unsigned int	Thread::get_corenumber()
{
	return (sysconf(_SC_NPROCESSORS_ONLN));
}

#endif

Thread::Thread() : thr(0)
{

}

Thread::Thread(void *(*function)(void*), void *data) : thr(0)
{
	create(function, data);
}

Thread::~Thread()
{
	exit(true);
}

bool	Thread::create(void *(*function)(void*), void *data)
{
	pthread_create(&thr, 0, f, data);
	if (!thr)
	{
		std::cerr << "error! pthread_create()" << std::endl;
		return (false);
	}
	return (true);
}

void	Thread::exit(bool const code)
{
	pthread_exit(code ? &thr : 0);
}

bool	Thread::wait()
{
	return (!pthread_join(thr, 0));
}

#endif
