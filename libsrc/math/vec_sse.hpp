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

#ifndef VEC_SSE_H_
#define VEC_SSE_H_

#include "xmmintrin.h"

template>
class	vec<float, 4>
{
	public:

		template<class V, unsigned int W>
		static vec	cast(vec<V, W> const &);

		union
		{
			float	ar[4];
			__m128	v;
		};


		float		&operator[](unsigned int const x);
		float const	&operator[](unsigned int const x) const;

		vec	&operator++();
		vec	operator++(int);
		vec	&operator--();
		vec	operator--(int);

		template<unsigned int V>
		vec	&operator=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator+=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator-=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator*=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator/=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator%=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator|=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator&=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator^=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator<<=(vec<float, V> const &x);
		template<unsigned int V>
		vec	&operator>>=(vec<float, V> const &x);

		vec	&operator=(float const &x);
		vec	&operator+=(float const &x);
		vec	&operator-=(float const &x);
		vec	&operator*=(float const &x);
		vec	&operator/=(float const &x);
		vec	&operator%=(float const &x);
		vec	&operator|=(float const &x);
		vec	&operator&=(float const &x);
		vec	&operator^=(float const &x);
		vec	&operator<<=(float const &x);
		vec	&operator>>=(float const &x);
};

template<class V, unsigned int W>
vec<float, 4>		vec<float, 4>::cast(vec<V, W> const &x)
{
	vec<float, 4>	a;

	for (unsigned int i = 0; i < 4; ++i)
		a.ar[i] = i < W ? (float)x.ar[i] : 0;
	return (a);
}

inline float	&vec<float, 4>::operator[](unsigned int const x)
{
	return (ar[x]);
}

inline float const	&vec<float, 4>::operator[](unsigned int const x) const
{
	return (ar[x]);
}

///////////////////////////////////////

inline vec<float, 4>	&vec<float, 4>::operator++()
{
	for (unsigned int i = 0; i < 4; ++i)
		++ar[i];
	return (*this);
}

inline vec<float, 4>		vec<float, 4>::operator++(int)
{
	vec<float, 4>	x;

	for (unsigned int i = 0; i < 4; ++i)
		x.ar[i] = ar[i]++;
	return (x);
}

inline vec<float, 4>	&vec<float, 4>::operator--()
{
	for (unsigned int i = 0; i < 4; ++i)
		--ar[i];
	return (*this);
}

inline vec<float, 4>		vec<float, 4>::operator--(int)
{
	vec<float, 4>	x;

	for (unsigned int i = 0; i < 4; ++i)
		x.ar[i] = ar[i]--;
	return (x);
}

///////////////////////////////////////

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] = x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator+=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] += x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator-=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] -= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator*=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] *= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator/=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] /= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator%=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] %= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator|=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] |= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator&=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] &= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator^=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] ^= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator<<=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] <<= x.ar[i];
	return (*this);
}

template<unsigned int V>
inline vec<float, 4>	&vec<float, 4>::operator>>=(vec<float, V> const &x)
{
	for (unsigned int i = 0; i < (4 > V ? V : 4); ++i)
		ar[i] >>= x.ar[i];
	return (*this);
}

///////////////////////////////////////

inline vec<float, 4>	&vec<float, 4>::operator=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] = x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator+=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] += x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator-=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] -= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator*=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] *= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator/=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] /= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator%=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] %= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator|=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] |= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator&=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] &= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator^=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] ^= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator<<=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] <<= x;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator>>=(float const &x)
{
	for (unsigned int i = 0; i < 4; ++i)
		ar[i] >>= x;
	return (*this);
}

///////////////////////////////////////////////////////////////////////////////

inline vec<float, 4>	operator+(vec<float, 4> const &x)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < 4; ++i)
		a.ar[i] = +x.ar[i];
	return (a);
}

inline vec<float, 4>	operator-(vec<float, 4> const &x)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < 4; ++i)
		a.ar[i] = -x.ar[i];
	return (a);
}

inline vec<float, 4>	operator~(vec<float, 4> const &x)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < 4; ++i)
		a.ar[i] = ~x.ar[i];
	return (a);
}

///////////////////////////////////////

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator+(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] + y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator-(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] - y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator*(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] * y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator/(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] / y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator%(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] % y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator|(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] | y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator&(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] & y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator^(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] ^ y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator<<(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] << y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	operator>>(vec<float, 4> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] >> y.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator<(vec<float, 4> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] >= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator>(vec<float, 4> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] <= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator<=(vec<float, 4> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] > y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U, unsigned int V>
inline bool	operator>=(vec<float, 4> const &x, vec<T, V> const &y)
{
	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		if (x.ar[i] < y.ar[i])
			return (false);
	return (true);
}

///////////////////////////////////////

template<class T, unsigned int U>
inline vec<float, 4>	operator+(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] + y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator-(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] - y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator*(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] * y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator/(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] / y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator%(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] % y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator|(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] | y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator&(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] & y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator^(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] ^ y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator<<(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] << y;
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator>>(vec<float, 4> const &x, T const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] >> y;
	return (a);
}

template<class T, unsigned int U>
inline bool	operator<(vec<float, 4> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar >= y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>(vec<float, 4> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar <= y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator<=(vec<float, 4> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar > y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>=(vec<float, 4> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar < y)
			return (false);
	return (true);
}

///////////////////////////////////////

template<class T, unsigned int U>
inline vec<float, 4>	operator+(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x + y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator-(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x - y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator*(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x * y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator/(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x / y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator%(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x % y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator|(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x | y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator&(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x & y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator^(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x ^ y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator<<(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x << y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<float, 4>	operator>>(T const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x >> y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline bool	operator<(T const &x, vec<float, 4> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x >= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>(T const &x, vec<float, 4> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x <= y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator<=(T const &x, vec<float, 4> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x > y.ar[i])
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>=(T const &x, vec<float, 4> const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x < y.ar[i])
			return (false);
	return (true);
}

#endif
