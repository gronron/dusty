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

#pragma once

#include <mutex>
#include <condition_variable>

#include "thread.hpp"

class	LightThreadPool
{
public:

	struct		Task
	{
		void	*(*function)(void*);
		void	*data;
	};

	template<class T, class U, class V>
	struct				Taskset
	{
		unsigned int	size;
		T				*data;
		U				*function;
		V				param;
	};

	struct				Worker
	{
		Thread			thrd;
		LightThreadPool	*ltp;
		Task			*tasks;
		unsigned int	front;
		unsigned int	back;
	};


	static LightThreadPool	&get_instance();
	static void				*_runthrd(void *);


	unsigned int		_thrdnbr;
	unsigned int		_queuesize;
	unsigned int		_looper;
	bool				_running;

	Worker				*_wrkrs;

	std::mutex				_mtx;
	std::condition_variable	_mcv;
	std::condition_variable	_scv;


	LightThreadPool(unsigned int const thrdnbr, unsigned int const queuesize);
	~LightThreadPool();

	
	void	add_task(void *(*function)(void*), void *data);
	void	run();
	
	template<template<class, class, class>class F, class T, class U, class V>
	void	run_tasks(unsigned int const size, T *data, U *function, V param);
};

template<template<class, class, class>class F, class T, class U, class V>
void					LightThreadPool::run_tasks(unsigned int const size, T *data, U *function, V param)
{
	Taskset<T, U, V>	a[8];

	unsigned int chunk = size / _thrdnbr;
	unsigned int left = size % _thrdnbr;
	unsigned int offset = 0;

	for (unsigned int i = 0; i < _thrdnbr; ++i)
	{
		a[i].size = chunk + (left ? left-- : 0);
		a[i].data = data + offset;
		offset += a[i].size;
		a[i].function = function;
		a[i].param = param;
		add_task(&F<T, U, V>::caller, a + i);
	}
	run();
}

#ifdef SECONDI

//setcontext posix

class	LightThreadPool
{
public:

	struct		Task
	{
		void	*(*function)(void*);
		void	*data;
	};

	struct				TasksGroup
	{
		Task const 		*tasks;
		unsigned int	tasksnbr;
		unsigned int	nexttask;
		int				waitingindex;
	};

	struct				WaitingFiber
	{
		union
		{
			Fiber		*fiber;
			int			next;
		};
		unsigned int	remainingchildtask;
	};

	Thread			*_threads;
	unsigned int	_thrdsnbr;

	TasksGroup		_tasksgroup[1024];
	unsigned int	_front;
	unsigned int	_back;
	//fifo task with prio // buffer tournant

	WaitingFiber	_waitingfibers[256];
	int				_free;
	//array of counter for task goup completion, can be associated with waiting fiber

	static LightThreadPool	&get_instance();
	static void				*_run_thread(void *);

	LightThreadPool(unsigned int const thrdnbr, unsigned int const queuesize);
	~LightThreadPool();

	void	add_tasks(unsigned int const, Task const * const, unsigned int * const);
	void	wait_tasks(unsigned int const);

};

#endif