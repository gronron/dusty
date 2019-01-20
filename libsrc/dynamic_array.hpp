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

		static constexpr unsigned int const default_size = 16;

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

		T &			operator[](unsigned int const);
		T const &	operator[](unsigned int const) const;

		void	reset(unsigned int const);
		void	reset(unsigned int const, unsigned int const, T const * const);
		void	reset();

		void	grow();

		T &		allocate();
		void	free(unsigned int const);
		void	free_keep_order(unsigned int const);

		void	clear();
};

template<class T>
inline DynamicArray<T>::DynamicArray()
	: number(0)
	, size(0)
	, data(nullptr)
{

}

template<class T>
inline DynamicArray<T>::DynamicArray(unsigned int const s)
	: number(0)
	, size(s)
	, data(::operator new(size * sizeof(T)))
{

}

template<class T>
inline DynamicArray<T>::DynamicArray(unsigned int const n, unsigned int const s, T const * const d)
	: number(n)
	, size(s)
	, data(::operator new(size * sizeof(T)))
{
	for (unsigned int i = 0; i < number; ++i)
		data[i].T(di]);
}

template<class T>
inline DynamicArray<T>::DynamicArray(DynamicArray const & x)
	: number(x.number)
	, size(x.size)
	, data(::operator new(size * sizeof(T)))
{
	for (unsigned int i = 0; i < number; ++i)
		data[i].T(x.data[i]);
}

template<class T>
inline DynamicArray<T>::DynamicArray(DynamicArray && x)
	: number(x.number)
	, size(x.size)
	, data(x.data)
{
	x.data = nullptr;
}

template<class T>
inline DynamicArray<T>::~DynamicArray()
{
	delete [] data;
}

template<class T>
inline DynamicArray<T> &	DynamicArray<T>::operator=(DynamicArray const & x)
{
	number = x.number;
	if (size < x.size)
	{
		delete [] data;
		data = ::operator new(size * sizeof(T));
	}
	size = x.size;
	for (unsigned int i = 0; i < number; ++i)
		data[i].T(x.data[i]);
}

template<class T>
inline DynamicArray<T> &	DynamicArray<T>::operator=(DynamicArray && x)
{
	number = x.number;
	size = x.size;
	delete [] data;
	data = x.data;
	x.data = nullptr;
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
void	DynamicArray<T>::reset(unsigned int const s)
{
	number = 0;
	size = s;
	delete [] data;
	data = ::operator new(size * sizeof(T));
}

template<class T>
void	DynamicArray<T>::reset(unsigned int const n, unsigned int const s, T const * const d)
{
	number = n;
	size = s;
	delete [] data;
	data = ::operator new(size * sizeof(T));
	for (unsigned int i = 0; i < number; ++i)
		data[i].T(d[i]);
	//memcpy(data, d, number * sizeof(T));
}

template<class T>
void	DynamicArray<T>::reset()
{
	number = 0;
	size = 0;
	delete [] data;
}

template<class T>
void	DynamicArray<T>::grow()
{
	if (size == 0)
		size = default_size;
	else
		size <<= 1;
	T * const new_data = ::operator new(size * sizeof(T));
	memcpy(new_data, data, number * sizeof(T));
	::operator delete(data);
	data = new_data;
}

template<class T>
inline T &	DynamicArray<T>::allocate()
{
	if (number >= size)
		grow();
	data[number].T();
	return (data[number++]);
}

template<class T>
inline void	DynamicArray<T>::free(unsigned int const x)
{
	if constexpr (std::is_class<T>::value || std::is_union<T>::value)
		data[x].~T();

	data[x] = data[--number];
}

template<class T>
inline void	DynamicArray<T>::free_keep_order(unsigned int const x)
{
	if constexpr (std::is_class<T>::value || std::is_union<T>::value)
		data[x].~T();

	--number;
	for (unsigned int i = x; i < number; ++i)
		data[i] = data[i + 1];
}

template<class T>
inline void	DynamicArray<T>::clear()
{
	if constexpr (std::is_class<T>::value || std::is_union<T>::value)
		for (unsigned int i = 0; i < number; ++i)
			data[i].~T();
	number = 0;
}
