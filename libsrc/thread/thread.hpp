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

#ifndef THREAD_H_
#define THREAD_H_

#if defined(_WIN32) || defined(__WIN32__)
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#else
	#include <pthread.h>
#endif

class	Thread
{
	public:

		#if defined(_WIN32) || defined(__WIN32__)
		HANDLE		thr;
		DWORD		thrid;
		#else
		pthread_t	thr;
		#endif

		static unsigned int	get_corenumber();


		Thread();
		Thread(void *(*function)(void*), void *data);
		~Thread();

		bool	create(void *(*function)(void*), void *data);
		void	exit(bool const code);
		bool	wait();
};

#endif
