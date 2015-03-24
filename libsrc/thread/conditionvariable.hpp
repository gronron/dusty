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

#ifndef CONDITIOVARIABLE_H_
#define CONDITIOVARIABLE_H_

#if defined(_WIN32) || defined(__WIN32__)
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
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

#endif
