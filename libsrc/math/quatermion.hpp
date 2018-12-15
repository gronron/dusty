#pragma once

#include "vec.hpp"

template<typename T>
using Quatermion = vec<T, 4>;

template<typename T>
Quatermion<T>	operator*(Quatermion<T> const &x, Quatermion<T> const y)
{
	//Quatermion<T>	a;
//
    //a.ar[3] = v3_dot(&qa->vector, &qb->vector);
//
    //v3_cross(&va, &qa->vector, &qb->vector);
    //v3_scalef(&vb, &qa->vector, &qb->scalar);
    //v3_scalef(&vc, &qb->vector, &qa->scalar);
    //v3_add(&va, &va, &vb);
    //v3_add(&qr->vector, &va, &vc);
//
    //quaternion_normalise(qr);
}

template<typename T>
Quatermion<T>	rotation(vec<T, 4> const &axis, T const angle)
{
	Quatermion<T>	a;

	a = axis * sin(angle * 0.5f);
	a[3] = cos(angle * 0.5f);

	return (vunit(a));
}
