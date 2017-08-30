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

#include "new.hpp"

template<class T>
class	Array
{
	public:
	
		unsigned int	number;
		unsigned int	size;
		T				*data;


		Array();
		Array(unsigned int const);
		Array(unsigned int const, unsigned int const, const T const *);
		Array(Array const &);
		Array(Array &&);
		~Array();
		
		void	operator()(unsigned int const, unsigned int const, const T const *);
		void	operator()();
		
		Array	&operator=(Array const &);
		Array	&operator=(Array &&);
		
		T		&operator[](unsigned int const);
		T const	&operator[](unsigned int const) const;
		
		T		&allocate();
		void	free(unsigned int const);
		
		void	add(T const &);
		void	remove(T const &);
};

template<class T>
inline Array<T>::Array() : number(0), size(0), data(0)
{
	
}

template<class T>
inline Array<T>::Array(unsigned int const s) : number(0), size(s), data(new T(size))
{

}

template<class T>
inline Array<T>::Array(unsigned int const s, const T const *d) : number(s), size(s), data(new T(size))
{
	memcpy(data, d, number);
}

template<class T>
inline Array<T>::Array(Array const &x) : number(x.number), size(x.size), data(new T(size))
{
	memcpy(data, x.data, number);
}

template<class T>
inline Array<T>::Array(Array &&x) : number(x.number), size(x.size), data(x.data)
{
	x.data = 0;
}

template<class T>
inline Array<T>::~Array()
{
	delete [] data;
}

template<class T>
void	Array<T>::operator()(unsigned int const, unsigned int const, const T const *)
{
	
}

template<class T>
void	Array<T>::operator()()
{
	
}

template<class T>
inline Array	&Array<T>::operator=(Array const &x)
{
	number = x.number;
	if (size < x.size)
	{
		if (data)
			delete [] data;
		data = new T[x.size];
	}
	size = x.size;
	memcpy(data, x.data, number);
}

template<class T>
inline Array	&Array<T>::operator=(Array &&x)
{
	number = x.number;
	size = x.size;
	if (data)
		delete [] data;
	data = x.data;
}

template<class T>
inline T		&Array<T>::operator[](unsigned int const x)
{
	return (data[x]);
}

template<class T>
inline T const	&Array<T>::operator[](unsigned int const x) const
{
	return (data[x]);
}

template<class T>
inline T		Array<T>::&allocate()
{
	if (number >= size)
	{
		size <<= 1;
		data = resize(data, number, size);
	}
	return (data[number++]);
}

template<class T>
inline void	Array<T>::free(unsigned int const x)
{
	data[x] = data[--number];
}

template<class T>
inline void	Array<T>::add(T const &x)
{
	if (number >= size)
	{
		size <<= 1;
		data = resize(data, number, size);
	}
	data[number++] = x;
}

template<class T>
inline void	Array<T>::remove(T const &x)
{
	int	index;
	for (index = number - 1; index >= 0 && data[index] != x; --index)
		;
	data[index] = data[--number];
}
