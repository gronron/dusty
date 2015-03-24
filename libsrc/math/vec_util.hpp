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

#ifndef VEC_UTIL_H_
#define VEC_UTIL_H_

#include <cmath>
#include "vec.hpp"

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	min(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>	a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a[i] = x.ar[i] > y.ar[i] ? y.ar[i] : x.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline vec<T, (U > V ? V : U)>	max(vec<T, U> const &x, vec<T, V> const &y)
{
	vec<T, (U > V ? V : U)>	a;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a[i] = x.ar[i] < y.ar[i] ? y.ar[i] : x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline vec<T, U>	abs(vec<T, U> const &x)
{
	vec<T, U>	a;

	for (unsigned int i = 0; i < U; ++i)
		a[i] = x.ar[i] < 0 ? -x.ar[i] : x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline T		sum(vec<T, U> const &x)
{
	T	a = 0;

	for (unsigned int i = 0; i < U; ++i)
		a += x.ar[i];
	return (a);
}

template<class T, unsigned int U>
inline T		sqrnorm(vec<T, U> const &x)
{
	T	a = 0;

	for (unsigned int i = 0; i < U; ++i)
		a += x.ar[i] * x.ar[i];
	return (a);
}

template<class T, unsigned int U, unsigned int V>
inline T		dot(vec<T, U> const &x, vec<T, V> const &y)
{
	T	a = 0;

	for (unsigned int i = 0; i < (U > V ? V : U); ++i)
		a += x.ar[i] * y.ar[i];
	return (a);
}

template<class T>
inline vec<T, 3>		cross(vec<T, 3> const &x, vec<T, 3> const &y)
{
	vec<T, 3>	a;

	a = x.ar[0] * y.ar[1] - x.ar[1] * y.ar[0];
	a = x.ar[1] * y.ar[2] - x.ar[2] * y.ar[1];
	a = x.ar[2] * y.ar[0] - x.ar[0] * y.ar[2];
	return (a);
}

namespace Sgl
{

	template<class T, unsigned int U>
	inline float		norm(vec<T, U> const &x)
	{
		float	a = 0.0f;

		for (unsigned int i = 0; i < U; ++i)
			a += x.ar[i] * x.ar[i];
		return (sqrt(a));
	}

	template<class T, unsigned int U>
	inline vec<T, U>	unit(vec<T, U> const &x)
	{
		float	a = 0.0f;

		for (unsigned int i = 0; i < U; ++i)
			a += x.ar[i] * x.ar[i];
		a = sqrt(a);
		return (a ? x / a : x * a);
	}

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
	inline vec<T, 3>	rot(vec<T, 3> const &x, vec<T, 3> const &y, float const z)
	{
		float const 	a = cos(z);
		float const 	b = sin(z);
		vec<T, 3> const	c = unit(y);
		vec<T, 3> const d = d * (1.0f - a);
		vec<T, 3>		e;

		e.ar[0] = x.ar[0] * (c.ar[0] * d.ar[0] + a) + x.ar[1] * (c.ar[0] * d.ar[1] - c.ar[2] * b) + x.ar[2] * (c.ar[0] * d.ar[2] + c.ar[1] * b),
		e.ar[1] = x.ar[0] * (c.ar[1] * d.ar[0] + c.ar[2] * b) + x.ar[1] * (c.ar[1] * d.ar[1] + a) + x.ar[2] * (c.ar[1] * d.ar[2] - c.ar[0] * b),
		e.ar[2] = x.ar[0] * (c.ar[2] * d.ar[0] - c.ar[1] * b) + x.ar[1] * (c.ar[2] * d.ar[1] + c.ar[0] * b) + x.ar[2] * (c.ar[2] * d.ar[2] + a);
		return (e);
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

	template<class T, unsigned int U>
	inline vec<T, U>	project(vec<T, U> const &x, vec<T, U> const &y)
	{
		vec<T, U> const	a = unit(y);

		return (x - a * dot(x, a));
	}

	template<class T, unsigned int U>
	inline vec<T, U>	reflect(vec<T, U> const &x, vec<T, U> const &y)
	{
		vec<T, U> const	a = unit(y);

		return (x - a * (dot(x, a) * 2.0f));
	}

}

#endif
