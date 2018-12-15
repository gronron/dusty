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

#include <cstring>

template<class T>
class	DynamicArray
{
	public:

		unsigned int	number;
		unsigned int	size;
		T *				data;


		DynamicArray();
		DynamicArray(unsigned int const);
		DynamicArray(unsigned int const, unsigned int const, T const * const);
		DynamicArray(DynamicArray const &);
		DynamicArray(DynamicArray &&);
		~DynamicArray();

		DynamicArray	&operator=(DynamicArray const &);
		DynamicArray	&operator=(DynamicArray &&);

		void	reset(unsigned int const);
		void	reset(unsigned int const, unsigned int const, T const * const);
		void	reset();

		T &			operator[](unsigned int const);
		T const &	operator[](unsigned int const) const;

		T &		allocate();
		void	free(unsigned int const);
		void	free_keep_order(unsigned int const);

		void	clear();
};

template<class T>
inline DynamicArray<T>::DynamicArray() : number(0), size(0), data(0)
{

}

template<class T>
inline DynamicArray<T>::DynamicArray(unsigned int const s) : number(0), size(s), data(new T[size])
{

}

template<class T>
inline DynamicArray<T>::DynamicArray(unsigned int const n, unsigned int const s, T const * const d) : number(n), size(s), data(new T[size])
{
	memcpy(data, d, number * sizeof(T));
}

template<class T>
inline DynamicArray<T>::DynamicArray(DynamicArray const & x) : number(x.number), size(x.size), data(new T[size])
{
	memcpy(data, x.data, number * sizeof(T));
}

template<class T>
inline DynamicArray<T>::DynamicArray(DynamicArray && x) : number(x.number), size(x.size), data(x.data)
{
	x.data = 0;
}

template<class T>
inline DynamicArray<T>::~DynamicArray()
{
	delete [] data;
}

template<class T>
inline DynamicArray<T> &	DynamicArray<T>::operator=(DynamicArray const &x)
{
	number = x.number;
	if (size < x.size)
	{
		delete [] data;
		data = new T[x.size];
	}
	size = x.size;
	memcpy(data, x.data, number * sizeof(T));
}

template<class T>
inline DynamicArray<T> &	DynamicArray<T>::operator=(DynamicArray &&x)
{
	number = x.number;
	size = x.size;
	delete [] data;
	data = x.data;
	x.data = 0;
}

template<class T>
void	DynamicArray<T>::reset(unsigned int const s)
{
	number = 0;
	size = s;
	delete [] data;
	data = new T[size];
}

template<class T>
void	DynamicArray<T>::reset(unsigned int const n, unsigned int const s, T const * const d)
{
	number = n;
	size = s;
	delete [] data;
	data = new T[size];
	memcpy(data, d, number * sizeof(T));
}

template<class T>
void	DynamicArray<T>::reset()
{
	number = 0;
	size = 0;
	delete [] data;
}

template<class T>
inline T &	DynamicArray<T>::operator[](unsigned int const x)
{
	return (data[x]);
}

template<class T>
inline T const &	DynamicArray<T>::operator[](unsigned int const x) const
{
	return (data[x]);
}

template<class T>
inline T &	DynamicArray<T>::allocate()
{
	if (number >= size)
	{
		size <<= 1;
		T * const new_data = new T[size];
		memcpy(new_data, data, number * sizeof(T));
		delete [] data;
		data = new_data;
	}
	return (data[number++]);
}

template<class T>
inline void	DynamicArray<T>::free(unsigned int const x)
{
	data[x] = data[--number];
}

template<class T>
inline void	DynamicArray<T>::free_keep_order(unsigned int const x)
{
	--number;
	for (unsigned int i = x; i < number; ++i)
		data[i] = data[i + 1];
}

template<class T>
inline void	DynamicArray<T>::clear()
{
	number = 0;
}
