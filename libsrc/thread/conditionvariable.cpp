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

#include "conditionvariable.hpp"

#if defined(_WIN32) || defined(__WIN32__)

Conditionvariable::Conditionvariable()
{
	InitializeConditionVariable(&_condvar);
}

Conditionvariable::~Conditionvariable()
{

}

void	Conditionvariable::sleep(Mutex &mtx)
{
	SleepConditionVariableCS(&_condvar, &mtx._mtx, INFINITE);
}

void	Conditionvariable::sleep(Mutex &mtx, double const timeout)
{
	
	SleepConditionVariableCS(&_condvar, &mtx._mtx, timeout * 1000.0);
}

void	Conditionvariable::wake()
{
	WakeConditionVariable(&_condvar);
}

void	Conditionvariable::wakeall()
{
	WakeAllConditionVariable(&_condvar);
}

#else

Conditionvariable::Conditionvariable()
{
	pthread_cond_init(&_condvar, 0);
}

Conditionvariable::~Conditionvariable()
{
	pthread_cond_destroy(&_condvar);
}

void	Conditionvariable::sleep(Mutex &mtx)
{
	pthread_cond_wait(&_condvar, &mtx._mtx);
}

void				Conditionvariable::sleep(Mutex &mtx, double const timeout)
{
	struct timespec	a;

	a.tv_sec = (int)timeout;
	a.tv_nsec = (int)((timeout - a.tv_sec) * 1000000000.0);
	pthread_cond_timedwait(&_condvar, &mtx._mtx, &a);
}

void	Conditionvariable::wake()
{
	pthread_cond_signal(&_condvar);
}

void	Conditionvariable::wakeall()
{
	pthread_cond_broadcast(&_condvar);
}

#endif
