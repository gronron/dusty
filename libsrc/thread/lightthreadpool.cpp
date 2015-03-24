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

#include "lightthreadpool.hpp"

void				*Lightthreadpool::_runthrd(void *data)
{
	Worker			*wrkr = (Worker *)data;
	Lightthreadpool	*ltp = wrkr->ltp;

	while (ltp->_running)
	{
		if (wrkr->front != wrkr->back)
		{
			wrkr->tasks[wrkr->back].function(wrkr->tasks[wrkr->back].data);
			wrkr->back = (wrkr->back + 1) % ltp->_queuesize;
			
		}
		else
		{
			ltp->_mtx.lock();
			ltp->_mcv.wake();
			ltp->_scv.sleep(ltp->_mtx);
			ltp->_mtx.unlock();
		}
	}
	return (0);
}

Lightthreadpool::Lightthreadpool(unsigned int n, unsigned int s) : _thrdnbr(n), _queuesize(s), _looper(0), _running(true)
{
	_wrkrs = new Worker[_thrdnbr];

	for (unsigned int i = 0; i < _thrdnbr; ++i)
	{
		_wrkrs[i].tasks = new Task[_queuesize];
		_wrkrs[i].front = 0;
		_wrkrs[i].back = 0;
		_wrkrs[i].thrd.create(Lightthreadpool::_runthrd, _wrkrs + i);
	}
}

Lightthreadpool::~Lightthreadpool()
{
	_running = false;
	for (unsigned int i = 0; i < _thrdnbr; ++i)
	{
		_wrkrs[i].thrd.wait();
		delete [] _wrkrs[i].tasks;
	}
	delete [] _wrkrs;
}

void	Lightthreadpool::addtask(void *(*function)(void*), void *data)
{
	if ((_wrkrs[_looper].front + 1) % _queuesize != _wrkrs[_looper].back)
	{
		_wrkrs[_looper].tasks[_wrkrs[_looper].front].function = function;
		_wrkrs[_looper].tasks[_wrkrs[_looper].front].data = data;
		_wrkrs[_looper].front = (_wrkrs[_looper].front + 1) % _queuesize;
	}
	else
		run();
	_looper = (_looper + 1) % _thrdnbr;
}

void	Lightthreadpool::run()
{
	bool	done = false;

	_mtx.lock();
	_scv.wakeall();
	while (!done)
	{
		done = true;
		for (unsigned int i = 0; i < _thrdnbr; ++i)
			done = done && (_wrkrs[i].front == _wrkrs[i].back);
		if (!done)
			_mcv.sleep(_mtx);
	}
	_mtx.unlock();
}
