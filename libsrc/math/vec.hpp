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

#ifndef VEC_H_
#define VEC_H_

template<class T, unsigned int U>
class vec
{
	public:

		template<class V, unsigned int W>
		static vec	cast(vec<V, W> const &);


		T	ar[U];


		T		&operator[](unsigned int const x);
		T const	&operator[](unsigned int const x) const;

		vec	&operator++();
		vec	operator++(int);
		vec	&operator--();
		vec	operator--(int);

		template<unsigned int V>
		vec	&operator=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator+=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator-=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator*=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator/=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator%=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator|=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator&=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator^=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator<<=(vec<T, V> const &x);
		template<unsigned int V>
		vec	&operator>>=(vec<T, V> const &x);

		vec	&operator=(T const &x);
		vec	&operator+=(T const &x);
		vec	&operator-=(T const &x);
		vec	&operator*=(T const &x);
		vec	&operator/=(T const &x);
		vec	&operator%=(T const &x);
		vec	&operator|=(T const &x);
		vec	&operator&=(T const &x);
		vec	&operator^=(T const &x);
		vec	&operator<<=(T const &x);
		vec	&operator>>=(T const &x);
};

template<class T, unsigned int U> template<class V, unsigned int W>
vec<T, U>		vec<T, U>::cast(vec<V, W> const &x)
{
	vec<T, U>	a;

	for (unsigned int i = 0; i < U; ++i)
		a.ar[i] = i < W ? (T)x.ar[i] : 0;
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
		if (x.ar >= y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>(vec<T, U> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar <= y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator<=(vec<T, U> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar > y)
			return (false);
	return (true);
}

template<class T, unsigned int U>
inline bool	operator>=(vec<T, U> const &x, T const &y)
{
	for (unsigned int i = 0; i < U; ++i)
		if (x.ar < y)
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

#endif
