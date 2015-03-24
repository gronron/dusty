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
		std::cerr << "Error: CreateThread()" << std::endl;
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
		std::cerr << "Error: pthread_create()" << std::endl;
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
