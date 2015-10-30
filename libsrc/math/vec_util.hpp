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

#ifndef VEC_UTIL_H_
#define VEC_UTIL_H_

#include <cmath>
#include "vec.hpp"

#undef min
#undef max

template<class T, unsigned int U>
inline vec<T, U>	vcall(T (*f)(T), vec<T, U> const &x)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = f(x.ar[i]);
	return (a);
}

template<class T>
inline T const	&min(T const &x, T const &y)
{
	return ((x > y) ? y : x);
}

template<class T>
inline T const	&max(T const &x, T const &y)
{
	return ((x < y) ? y : x);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	vmin(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] > y.ar[i] ? y.ar[i] : x.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	vmax(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>		a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a.ar[i] = x.ar[i] < y.ar[i] ? y.ar[i] : x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	vabs(vec<T, U> const &x)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = x.ar[i] < 0 ? -x.ar[i] : x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	vsqrt(vec<T, U> const &x)
{
	vec<T, U>		a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = (T)sqrt(x.ar[i]);
	return (a);
}

template<class T, unsigned int U>
inline T	vsum(vec<T, U> const &x)
{
	T		a = 0;

	for (unsigned int i = 0; i < U; ++i)
		a += x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline T	vsqrnorm(vec<T, U> const &x)
{
	T		a = 0;

	for (unsigned int i = 0; i < U; ++i)
		a += x.ar[i] * x.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline T	vdot(vec<T, U> const &x, vec<T, V> const &y)
{
	T		a = 0;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a += x.ar[i] * y.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	vcross(vec<T, U> const &x, vec<T, U> const &y)
{
	vec<T, U>		a;

	a.ar[0] = x.ar[1] * y.ar[2] - x.ar[2] * y.ar[1];
	a.ar[1] = x.ar[2] * y.ar[0] - x.ar[0] * y.ar[2];
	a.ar[2] = x.ar[0] * y.ar[1] - x.ar[1] * y.ar[0];
	for (unsigned int i = 3; i < U; ++i)
		a.ar[i] = (T)0;
	return (a);
}

template<class T, class U, unsigned int V>
inline T	vnorm(vec<U, V> const &x)
{
	T	a = 0;

	for (unsigned int i = 0; i < V; ++i)
		a += x.ar[i] * x.ar[i];
	return ((T)sqrt(a));
}

template<class T, class U, unsigned int V>
inline vec<U, V>	vunit(vec<U, V> const &x)
{
	T	a = 0;

	for (unsigned int i = 0; i < V; ++i)
		a += x.ar[i] * x.ar[i];
	a = (T)sqrt(a);
	return (a ? x / a : x * a);
}

template<class T, unsigned int U>
inline vec<T, U>	vproject(vec<T, U> const &x, vec<T, U> const &y)
{
	return (x - y * dot(x, y));
}

template<class T, class U, unsigned int V>
inline vec<T, V>	vunit_project(vec<U, V> const &x, vec<U, V> const &y)
{
	vec<T, U> const	a = vunit<T>(y);

	return (x - a * dot(x, a));
}

template<class T, unsigned int U>
inline vec<T, U>	vreflect(vec<T, U> const &x, vec<T, U> const &y)
{
	return (x - y * (dot(x, y) * 2.0f));
}

template<class T, class U, unsigned int V>
inline vec<T, V>	vunit_reflect(vec<U, V> const &x, vec<U, V> const &y)
{
	vec<T, V> const	a = vunit<T>(y);

	return (x - a * (dot(x, a) * 2.0f));
}

template<class T, class U>
inline vec<U, 3>	vrot(vec<U, 3> const &x, vec<U, 3> const &y, T const z)
{
	T const 	a = (T)cos(z);
	T const 	b = (T)sin(z);
	vec<U, 3> const	c = vunit<T>(y);
	vec<U, 3> const d = d * (1.0f - a);
	vec<U, 3>		e;

	e.ar[0] = x.ar[0] * (c.ar[0] * d.ar[0] + a) + x.ar[1] * (c.ar[0] * d.ar[1] - c.ar[2] * b) + x.ar[2] * (c.ar[0] * d.ar[2] + c.ar[1] * b),
	e.ar[1] = x.ar[0] * (c.ar[1] * d.ar[0] + c.ar[2] * b) + x.ar[1] * (c.ar[1] * d.ar[1] + a) + x.ar[2] * (c.ar[1] * d.ar[2] - c.ar[0] * b),
	e.ar[2] = x.ar[0] * (c.ar[2] * d.ar[0] - c.ar[1] * b) + x.ar[1] * (c.ar[2] * d.ar[1] + c.ar[0] * b) + x.ar[2] * (c.ar[2] * d.ar[2] + a);
	return (e);
}

namespace Sgl
{
	template<class T>
	inline float			ang(vec<T, 2> const &x)
	{
		float const	a = sqrt(x.ar[0] * x.ar[0] + x.ar[1] * x.ar[1]);
		float const	b = acos(a ? x.ar[0] / a : 0.0f);

		return (x.ar[1] < 0 ? -b : b);
	}

	template<class T>
	inline float			ang(vec<T, 3> const &x, vec<T, 3> const &y)
	{
		float const	a = norm(x) * norm(y);

		return (acos(a ? dot(x, y) / a : 0.0f));
	}

	template<class T>
	inline float			angx(vec<T, 3> const &x)
	{
		float const	a = sqrt(x.ar[1] * x.ar[1] + x.ar[2] * x.ar[2]);
		float const	b = acos(a ? x.ar[1] / a : 0.0f);

		return (x.ar[2] < 0 ? -b : b);
	}

	template<class T>
	inline float			angy(vec<T, 3> const &x)
	{
		float const a = sqrt(x.ar[2] * x.ar[2] + x.ar[0] * x.ar[0]);
		float const b = acos(a ? x.ar[2] / a : 0.0f);

		return (x.ar[0] < 0 ? -b : b);
	}

	template<class T>
	float			angz(vec<T, 3> const &x)
	{
		float const	a = sqrt(x.ar[0] * x.ar[0] + x.ar[1] * x.ar[1]);
		float const	b = acos(a ? x.ar[0] / a : 0.0f);

		return (x.ar[1] < 0 ? -b : b);
	}

	template<class T>
	inline vec<T, 2>	rot(vec<T, 2> const &x, float const y)
	{
		float const		a = cos(y);
		float const		b = sin(y);
		vec<T, 2>		c;

		c.ar[0] = x.ar[0] * a - x.ar[1] * b;
		c.ar[1] = x.ar[0] * b + x.ar[1] * a;
		return (c);
	}

	template<class T>
	inline vec<T, 3>	rotx(vec<T, 3> const &x, float const y)
	{
		float const 	a = cos(y);
		float const 	b = sin(y);
		vec<T, 3>		c;

		c.ar[0] = x.ar[0];
		c.ar[1] = x.ar[1] * a - x.ar[2] * b;
		c.ar[2] = x.ar[1] * b + x.ar[2] * a;
		return (c);
	}

	template<class T>
	inline vec<T, 3>	roty(vec<T, 3> const &x, float const y)
	{
		float const 	a = cos(y);
		float const 	b = sin(y);
		vec<T, 3>		c;

		c.ar[0] = x.ar[2] * b + x.ar[0] * a;
		c.ar[1] = x.ar[1];
		c.ar[2] = x.ar[2] * a - x.ar[0] * b;
		return (c);
	}

	template<class T>
	inline vec<T, 3>	rotz(vec<T, 3> const &x, float const y)
	{
		float const		a = cos(y);
		float const		b = sin(y);
		vec<T, 3>		c;

		c.ar[0] = x.ar[0] * a - x.ar[1] * b;
		c.ar[1] = x.ar[0] * b + x.ar[1] * a;
		c.ar[2] = x.ar[2];
		return (c);
	}

}

#endif
