#pragma once

#include "vec.hpp"

template<typename T, unsigned int X, unsigned int Y>
struct	Matrix
{
	vec<T, X>	ar[Y];

	vec<T, X> &			operator[](unsigned int const x) { return (ar[x]); }
	vec<T, X> const &	operator[](unsigned int const x) const { return (ar[x]); }
};

///////////////////////////////////////

template<typename T, unsigned int X, unsigned int Y, unsigned int Z, unsigned int W>
Matrix<T, Y, Z>	operator*(Matrix<T, X, Y> const & x, Matrix<T, Z, W> const & y)
{
	constexpr unsigned int const A = X < W ? X : W;

	Matrix<T, Y, Z>	a;

	for (unsigned int k = 0; k < Z; ++k)
	{
		for (unsigned int j = 0; j < Y; ++j)
		{
			a[k][j] = (T)0;
			for (unsigned int i = 0; i < A; ++i)
				a[k][j] += x[j][i] * y[i];
		}
	}
}

template<typename T, unsigned int U, unsigned int X, unsigned int Y>
vec<T, U>		operator*(Matrix<T, X, Y> const & m, vec<T, U> const & v)
{
	vec<T, U>	a;

	for (unsigned int j = 0; j < U; ++j)
	{
		a[j] = (T)0;
		for (unsigned int i = 0; i < U; ++i)
			a[j] += m[j][i] * v[i];
	}

	return (a);
}

template<typename T, unsigned int X, unsigned int Y>
Matrix<T, Y, X>	transpose(Matrix<T, X, Y> const & m)
{
	Matrix<T, Y, X>	a;

	for (unsigned int j = 0; j < Y; ++j)
		for (unsigned int i = 0; i < X; ++i)
			a[i][j] = m[j][i];

	return (a);
}


template<typename T, unsigned int X, unsigned int Y>
Matrix<T, Y, X>	indentity()
{
	Matrix<T, Y, X>	a;

	for (unsigned int j = 0; j < Y; ++j)
		for (unsigned int i = 0; i < X; ++i)
			a[j][i] = (T)0;

	for (unsigned int i = 0; i < Y; ++i)
		a[i][i] = (T)1;

	return (a);
}
