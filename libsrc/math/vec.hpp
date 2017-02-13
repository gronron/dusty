/******************************************************************************
Copyright (c) 2015-2017, Geoffrey TOURON
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

#ifndef VEC_H_
#define VEC_H_

template<class T, unsigned int U>
struct	vec
{
	T	ar[U];


	template<class V, unsigned int W>
	explicit operator vec<V, W>();
	template<class V, unsigned int W>
	explicit operator vec<V, W>() const;

	T		&operator[](unsigned int const);
	T const	&operator[](unsigned int const) const;

	vec	&operator++();
	vec	operator++(int);
	vec	&operator--();
	vec	operator--(int);

	template<unsigned int V>
	vec	&operator=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator+=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator-=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator*=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator/=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator%=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator|=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator&=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator^=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator<<=(vec<T, V> const &);
	template<unsigned int V>
	vec	&operator>>=(vec<T, V> const &);

	vec	&operator=(T const &);
	vec	&operator+=(T const &);
	vec	&operator-=(T const &);
	vec	&operator*=(T const &);
	vec	&operator/=(T const &);
	vec	&operator%=(T const &);
	vec	&operator|=(T const &);
	vec	&operator&=(T const &);
	vec	&operator^=(T const &);
	vec	&operator<<=(T const &);
	vec	&operator>>=(T const &);
};

template<class T, unsigned int U> template<class V, unsigned int W>
vec<T, U>::operator vec<V, W>()
{
	vec<V, W>	a;

	for (unsigned int i = 0; i < W; ++i)
		a.ar[i] = (V)(i < U ? ar[i] : 0);
	return (a);
}

template<class T, unsigned int U> template<class V, unsigned int W>
vec<T, U>::operator vec<V, W>() const
{
	vec<V, W>	a;

	for (unsigned int i = 0; i < W; ++i)
		a.ar[i] = (V)(i < U ? ar[i] : 0);
	return (a);
}

template<class T, unsigned int U>
inline T	&vec<T, U>::operator[](unsigned int const x)
{
	return (ar[x]);
}

template<class T, unsigned int U>
inline T const	&vec<T, U>::operator[](unsigned int const x) const
{
	return (ar[x]);
}

///////////////////////////////////////

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator++()
{
	for (unsigned int i = 0; i < U; ++i)
		++ar[i];
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>		vec<T, U>::operator++(int)
{
	vec<T, U>	x;

	for (unsigned int i = 0; i < U; ++i)
		x.ar[i] = ar[i]++;
	return (x);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator--()
{
	for (unsigned int i = 0; i < U; ++i)
		--ar[i];
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>		vec<T, U>::operator--(int)
{
	vec<T, U>	x;

	for (unsigned int i = 0; i < U; ++i)
		x.ar[i] = ar[i]--;
	return (x);
}

///////////////////////////////////////

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] = x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator+=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] += x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator-=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] -= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator*=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] *= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator/=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] /= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator%=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] %= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator|=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] |= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator&=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] &= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator^=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] ^= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator<<=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] <<= x.ar[i];
	return (*this);
}

template<class T, unsigned int U> template<unsigned int V>
inline vec<T, U>	&vec<T, U>::operator>>=(vec<T, V> const &x)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		ar[i] >>= x.ar[i];
	return (*this);
}

///////////////////////////////////////

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] = x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator+=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] += x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator-=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] -= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator*=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] *= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator/=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] /= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator%=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] %= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator|=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] |= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator&=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] &= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator^=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] ^= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator<<=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] <<= x;
	return (*this);
}

template<class T, unsigned int U>
inline vec<T, U>	&vec<T, U>::operator>>=(T const &x)
{
	for (unsigned int i = 0; i < U; ++i)
		ar[i] >>= x;
	return (*this);
}

///////////////////////////////////////////////////////////////////////////////

template<class T, unsigned int U>
inline vec<T, U>	operator+(vec<T, U> const &x)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = +x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator-(vec<T, U> const &x)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = -x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator~(vec<T, U> const &x)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = ~x.ar[i];
	return (a);
}

///////////////////////////////////////

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator+(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] + y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator-(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] - y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator*(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] * y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator/(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] / y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator%(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] % y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator|(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] | y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator&(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] & y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator^(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] ^ y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator<<(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] << y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator>>(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] >> y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator<(vec<T, U> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] >= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator>(vec<T, U> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] <= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator<=(vec<T, U> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] > y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator>=(vec<T, U> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] < y.ar[i])
			return (false);
	return (true);
}

///////////////////////////////////////

template<class T, unsigned int U>
inline vec<T, U>	operator+(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] + y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator-(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] - y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator*(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] * y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator/(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] / y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator%(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] % y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator|(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] | y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator&(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] & y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator^(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] ^ y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator<<(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] << y;
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator>>(vec<T, U> const &x, T const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] >> y;
	return (a);
}

template<class T, unsigned int U>
inline bool	operator<(vec<T, U> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar[i] >= y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>(vec<T, U> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar[i] <= y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator<=(vec<T, U> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar[i] > y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>=(vec<T, U> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar[i] < y)
			return (false);
	return (true);
}

///////////////////////////////////////

template<class T, unsigned int U>
inline vec<T, U>	operator+(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x + y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator-(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x - y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator*(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x * y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator/(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x / y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator%(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x % y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator|(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x | y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator&(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x & y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator^(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x ^ y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator<<(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x << y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	operator>>(T const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x >> y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline bool	operator<(T const &x, vec<T, U> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x >= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>(T const &x, vec<T, U> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x <= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator<=(T const &x, vec<T, U> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x > y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>=(T const &x, vec<T, U> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x < y.ar[i])
			return (false);
	return (true);
}

#include "vec_sse.hpp"

#endif
