/******************************************************************************
Copyright (c) 2015-2017, Geoffrey TOURON
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

#ifndef MUTEX_H_
#define MUTEX_H_

#if defined(_WIN32) || defined(__WIN32__)
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#undef near
	#undef far
#else
	#include <pthread.h>
#endif

class	Mutex
{
	public:

		#if defined(_WIN32) || defined(__WIN32__)
		CRITICAL_SECTION	_mtx;
		#else
		pthread_mutex_t		_mtx;
		#endif

		Mutex();
		~Mutex();

		void	lock();
		bool	trylock();
		void	unlock();
};

#if defined(_WIN32) || defined(__WIN32__)

inline Mutex::Mutex()
{
	InitializeCriticalSection(&_mtx);
}

inline Mutex::~Mutex()
{
	DeleteCriticalSection(&_mtx);
}

inline void	Mutex::lock()
{
	EnterCriticalSection(&_mtx);
}

inline bool	Mutex::trylock()
{
	return (TryEnterCriticalSection(&_mtx) != 0);
}

inline void	Mutex::unlock()
{
	LeaveCriticalSection(&_mtx);
}

#else

inline Mutex::Mutex()
{
	pthread_mutexattr_t	attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&_mtx, &attr);
	pthread_mutexattr_destroy(&attr);
}

inline Mutex::~Mutex()
{
	pthread_mutex_destroy(&_mtx);
}

inline void	Mutex::lock()
{
	pthread_mutex_lock(&_mtx);
}

inline bool	Mutex::trylock()
{
	return (!pthread_mutex_trylock(&_mtx));
}

inline void	Mutex::unlock()
{
	pthread_mutex_unlock(&_mtx);
}

#endif

#endif
