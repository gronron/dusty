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

#include "mutex.hpp"

#if defined(_WIN32) || defined(__WIN32__)

Mutex::Mutex()
{
	InitializeCriticalSection(&_mtx);
}

Mutex::~Mutex()
{
	DeleteCriticalSection(&_mtx);
}

void	Mutex::lock()
{
	EnterCriticalSection(&_mtx);
}

bool	Mutex::trylock()
{
	return (TryEnterCriticalSection(&_mtx));
}

void	Mutex::unlock()
{
	LeaveCriticalSection(&_mtx);
}

#else

Mutex::Mutex()
{
	pthread_mutexattr_t	attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&_mtx, &attr);
	pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&_mtx);
}

void	Mutex::lock()
{
	pthread_mutex_lock(&_mtx);
}

bool	Mutex::trylock()
{
	return (!pthread_mutex_trylock(&_mtx));
}

void	Mutex::unlock()
{
	pthread_mutex_unlock(&_mtx);
}

#endif
