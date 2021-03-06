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

#include "math/vec_util.hpp"
#include "shape.hpp"
#include "narrowphase.hpp"

//sphere-sphere

float					sphere_sphere(Body const *x, Body const *y, float const time, vec<float, 4> &normal)
{
	vec<float, 4> const	xp = x->position + x->velocity * (time - x->time);
	vec<float, 4> const	yp = y->position + y->velocity * (time - y->time);
	vec<float, 4> const	p = xp - yp;
	vec<float, 4> const	v = x->velocity - y->velocity;

	float const	r = ((Sphereshape const *)x->shape)->radius + ((Sphereshape const *)x->shape)->radius;
    float const pp = vsqrnorm(p) - r * r;

	if (pp <= 0.0)
		return (0.0f);

    float const	pv = vdot(p, v);

    if (pv >= 0.0)
        return (-1.0f);
	float const	vv = vsqrnorm(v);

	if ((pv + vv) <= 0.0f && (vv + 2.0f * pv + pp) >= 0.0f) //beware
		return (-1.0f);

	normal = 1.0f;
	return (pp + -pv * (pv / vv));
}

//sphere-abox

float					sphere_aabox(Body const *x, Body const *y, float const time, vec<float, 4> &normal)
{
	return (-1.0f);
}

//abox-abox

float					aabox_aabox(Body const *x, Body const *y, float const time, vec<float, 4> &normal)
{
	vec<float, 4> const	xp = x->position + x->velocity * (time - x->time);
	vec<float, 4> const	yp = y->position + y->velocity * (time - y->time);

	vec<float, 4> const	ivelocity = 1.0f / (y->velocity - x->velocity);
	vec<float, 4> const	p = xp - yp;
	vec<float, 4> const	du = p - ((Axisalignedboxshape const *)y->shape)->size;
	vec<float, 4> const	dv = p + ((Axisalignedboxshape const *)x->shape)->size;
	vec<float, 4> const	u = du * ivelocity;
	vec<float, 4> const	v = dv * ivelocity;

	float tnear = -INFINITY;
	float tfar = +INFINITY;

	int	n = 0;

	for (int i = 0; i < 3; ++i)
	{
		if (ivelocity[i] > 0.0f)
		{
			if (tnear < u[i])
				n = i;
			tnear = max(tnear, u[i]);
			tfar = min(tfar, v[i]);
		}
		else if (ivelocity[i] < 0.0f)
		{
			if (tnear < v[i])
				n = i;
			tnear = max(tnear, v[i]);
			tfar = min(tfar, u[i]);
		}
		else if (du[i] > 0.0f || dv[i] < 0.0f)
			return (-1.0f);
	}

	normal = 0.0f;
	normal[n] = 1.0f;
	return (tnear <= tfar ? tnear : -1.0f);
}

//cylinder-cylinder

//box-box

//sphere-cylinder

//sphere-box
//cylinder-abox
//cylinder-box
//abox-box
