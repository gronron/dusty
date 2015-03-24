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
