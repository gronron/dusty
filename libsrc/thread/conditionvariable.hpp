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

#ifndef CONDITIOVARIABLE_H_
#define CONDITIOVARIABLE_H_

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

#include "mutex.hpp"

class	Conditionvariable
{
	public:

		#if defined(_WIN32) || defined(__WIN32__)
		CONDITION_VARIABLE	_condvar;
		#else
		pthread_cond_t		_condvar;
		#endif

		Conditionvariable();
		~Conditionvariable();

		void	sleep(Mutex &);
		void	sleep(Mutex &, double const timeout);
		void	wake();
		void	wakeall();

};

#if defined(_WIN32) || defined(__WIN32__)

inline Conditionvariable::Conditionvariable()
{
	InitializeConditionVariable(&_condvar);
}

inline Conditionvariable::~Conditionvariable()
{

}

inline void	Conditionvariable::sleep(Mutex &mtx)
{
	SleepConditionVariableCS(&_condvar, &mtx._mtx, INFINITE);
}

inline void	Conditionvariable::sleep(Mutex &mtx, double const timeout)
{
	SleepConditionVariableCS(&_condvar, &mtx._mtx, (int)(timeout * 1000.0));
}

inline void	Conditionvariable::wake()
{
	WakeConditionVariable(&_condvar);
}

inline void	Conditionvariable::wakeall()
{
	WakeAllConditionVariable(&_condvar);
}

#else

inline Conditionvariable::Conditionvariable()
{
	pthread_cond_init(&_condvar, 0);
}

inline Conditionvariable::~Conditionvariable()
{
	pthread_cond_destroy(&_condvar);
}

inline void	Conditionvariable::sleep(Mutex &mtx)
{
	pthread_cond_wait(&_condvar, &mtx._mtx);
}

inline void			Conditionvariable::sleep(Mutex &mtx, double const timeout)
{
	struct timespec	a;

	a.tv_sec = (int)timeout;
	a.tv_nsec = (int)((timeout - a.tv_sec) * 1000000000.0);
	pthread_cond_timedwait(&_condvar, &mtx._mtx, &a);
}

inline void	Conditionvariable::wake()
{
	pthread_cond_signal(&_condvar);
}

inline void	Conditionvariable::wakeall()
{
	pthread_cond_broadcast(&_condvar);
}

#endif

#endif
