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
#include <cstdlib>

template<class T>
T		*resize(T *ptr, unsigned int const oldsize, unsigned int const newsize)
{
	T	*a;

	if (!(a = new T[newsize]))
		exit(EXIT_FAILURE);
	memcpy(a, ptr, (oldsize < newsize ? oldsize : newsize) * sizeof(T));
	delete [] ptr;

	return (a);
}

template<class T>
T		**new_matrix(unsigned int const x, unsigned int const y)
{
	T	**a;

	if (!(a = new T*[x]) || !(*a = new T[x * y]))
		exit(EXIT_FAILURE);
	for (unsigned int i = 1; i < x; ++i)
		a[i] = *a + i * x;

	return (a);
}

template<class T>
void	delete_matrix(T **a)
{
	delete [] *a;
	delete [] a;
}

template<class T>
T		***new_space(unsigned int const x, unsigned int const y, unsigned int const z)
{
	T	***a;

	if (!(a = new T**[x]) || !(*a = new T*[x * y]) || !(**a = new T[x * y * z]))
		exit(EXIT_FAILURE);
	for (unsigned int j = 0; j < x; ++j)
	{
		a[j] = *a + j * x;
		unsigned int const	b = j * x * y;
		for (unsigned int i = 0; i < y; ++i)
			a[j][i] = **a + b + i * y;
	}
	return (a);
}

template<class T>
void	delete_space(T ***a)
{
	delete [] **a;
	delete [] *a;
	delete [] a;
}
