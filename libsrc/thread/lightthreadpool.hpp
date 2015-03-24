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

#ifndef LIGHTTHREADPOOL_H_
#define LIGHTTHREADPOOL_H_

#include "thread.hpp"
#include "mutex.hpp"
#include "conditionvariable.hpp"

class	Lightthreadpool
{
	public:

		struct		Task
		{
			void	*(*function)(void*);
			void	*data;
		};

		struct				Worker
		{
			Thread			thrd;
			Lightthreadpool	*ltp;
			Task			*tasks;
			unsigned int	front;
			unsigned int	back;
		};


		static	void		*_runthrd(void *);


		unsigned int		_thrdnbr;
		unsigned int		_queuesize;
		unsigned int		_looper;
		bool				_running;

		Worker				*_wrkrs;

		Mutex				_mtx;
		Conditionvariable	_mcv;
		Conditionvariable	_scv;


		Lightthreadpool(unsigned int thrdnbr, unsigned int queuesize);
		~Lightthreadpool();

		void	addtask(void *(*function)(void*), void *data);
		void	run();
};

#endif
