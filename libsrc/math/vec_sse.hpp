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

template<>
struct	vec<float, 4>
{
	union
	{
		float	ar[4];
		__m128	v;
	};


	template<class V, unsigned int W>
	operator vec<V, W>();
	template<class V, unsigned int W>
	operator vec<V, W>() const;

	float		&operator[](unsigned int const x);
	float const	&operator[](unsigned int const x) const;

	vec	&operator=(vec<float, 4> const &x);
	vec	&operator+=(vec<float, 4> const &x);
	vec	&operator-=(vec<float, 4> const &x);
	vec	&operator*=(vec<float, 4> const &x);
	vec	&operator/=(vec<float, 4> const &x);
	
	vec	&operator=(float const &x);
	vec	&operator+=(float const &x);
	vec	&operator-=(float const &x);
	vec	&operator*=(float const &x);
	vec	&operator/=(float const &x);
};


template<class V, unsigned int W>
vec<float, 4>::operator vec<V, W>()
{
	vec<V, W>	a;

	for (unsigned int i = 0; i < W; ++i)
		a.ar[i] = (V)(i < 4 ? ar[i] : 0);
	return (a);
}

template<class V, unsigned int W>
vec<float, 4>::operator vec<V, W>() const
{
	vec<V, W>	a;

	for (unsigned int i = 0; i < W; ++i)
		a.ar[i] = (V)(i < 4 ? ar[i] : 0);
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

inline vec<float, 4>	&vec<float, 4>::operator=(vec<float, 4> const &x)
{
	v = x.v;
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator+=(vec<float, 4> const &x)
{
	v = _mm_add_ps(v, x.v);
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator-=(vec<float, 4> const &x)
{
	v = _mm_sub_ps(v, x.v);
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator*=(vec<float, 4> const &x)
{
	v = _mm_mul_ps(v, x.v);
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator/=(vec<float, 4> const &x)
{
	v = _mm_div_ps(v, x.v);
	return (*this);
}

///////////////////////////////////////

inline vec<float, 4>	&vec<float, 4>::operator=(float const &x)
{
	v = _mm_set1_ps(x);
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator+=(float const &x)
{
	v = _mm_add_ps(v, _mm_set1_ps(x));
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator-=(float const &x)
{
	v = _mm_sub_ps(v, _mm_set1_ps(x));
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator*=(float const &x)
{
	v = _mm_mul_ps(v, _mm_set1_ps(x));
	return (*this);
}

inline vec<float, 4>	&vec<float, 4>::operator/=(float const &x)
{
	v = _mm_div_ps(v, _mm_set1_ps(x));
	return (*this);
}

///////////////////////////////////////////////////////////////////////////////

inline vec<float, 4>	operator+(vec<float, 4> const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_add_ps(x.v, y.v);
	return (a);
}

inline vec<float, 4>	operator-(vec<float, 4> const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_sub_ps(x.v, y.v);
	return (a);
}

inline vec<float, 4>	operator*(vec<float, 4> const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_mul_ps(x.v, y.v);
	return (a);
}

inline vec<float, 4>	operator/(vec<float, 4> const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_div_ps(x.v, y.v);
	return (a);
}

///////////////////////////////////////

inline vec<float, 4>	operator+(vec<float, 4> const &x, float const &y)
{
	vec<float, 4>		a;

	a.v = _mm_add_ps(x.v, _mm_set1_ps(y));
	return (a);
}

inline vec<float, 4>	operator-(vec<float, 4> const &x, float const &y)
{
	vec<float, 4>		a;

	a.v = _mm_sub_ps(x.v, _mm_set1_ps(y));
	return (a);
}

inline vec<float, 4>	operator*(vec<float, 4> const &x, float const &y)
{
	vec<float, 4>		a;

	a.v = _mm_mul_ps(x.v, _mm_set1_ps(y));
	return (a);
}

inline vec<float, 4>	operator/(vec<float, 4> const &x, float const &y)
{
	vec<float, 4>		a;

	a.v = _mm_div_ps(x.v, _mm_set1_ps(y));
	return (a);
}

///////////////////////////////////////

inline vec<float, 4>	operator+(float const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_add_ps(_mm_set1_ps(x), y.v);
	return (a);
}

inline vec<float, 4>	operator-(float const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_sub_ps(_mm_set1_ps(x), y.v);
	return (a);
}

inline vec<float, 4>	operator*(float const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_mul_ps(_mm_set1_ps(x), y.v);
	return (a);
}

inline vec<float, 4>	operator/(float const &x, vec<float, 4> const &y)
{
	vec<float, 4>		a;

	a.v = _mm_div_ps(_mm_set1_ps(x), y.v);
	return (a);
}

///////////////////////////////////////////////////////////////////////////////

#undef min
#undef max

inline vec<float, 4>	min(vec<float, 4> const &x, vec<float, 4> const &y)
{
	vec<float, 4>	a;
	
	a.v = _mm_min_ps(x.v, y.v);
	return (a);
}

inline vec<float, 4>	max(vec<float, 4> const &x, vec<float, 4> const &y)
{
	vec<float, 4>	a;
	
	a.v = _mm_max_ps(x.v, y.v);
	return (a);
}

inline vec<float, 4>	sqrt(vec<float, 4> const &x)
{
	vec<float, 4>	a;
	
	a.v = _mm_sqrt_ps(x.v);
	return (a);
}

#endif
