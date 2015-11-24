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

#include "shape.hpp"

Shape::Shape(char const t) : type(t)
{

}

Shape::~Shape()
{

}

///////////////////////////////////////

Sphereshape::Sphereshape() : Shape(SPHERE)
{

}

Sphereshape::~Sphereshape()
{

}

Aabb	&Sphereshape::compute_aabb(Aabb &aabb, vec<float, 4> const &position) const
{
	aabb.bottom = position - radius;
	aabb.top = position + radius;
	return (aabb);
}

///////////////////////////////////////

Axiscylindershape::Axiscylindershape() : Shape(AXIS_CYLINDER)
{

}

Axiscylindershape::~Axiscylindershape()
{

}

Aabb	&Axiscylindershape::compute_aabb(Aabb &aabb, vec<float, 4> const &position) const
{
	aabb.bottom = position - radius;
	aabb.top = position + radius;
	aabb.bottom[axis] = position[axis] - lenght;
	aabb.top[axis] = position[axis] + lenght;
	return (aabb);
}

///////////////////////////////////////

Axisalignedboxshape::Axisalignedboxshape() : Shape(AXIS_ALIGNED_BOX)
{

}

Axisalignedboxshape::~Axisalignedboxshape()
{

}

Aabb	&Axisalignedboxshape::compute_aabb(Aabb &aabb, vec<float, 4> const &position) const
{
	aabb.bottom = position - size;
	aabb.top = position + size;
	return (aabb);
}

///////////////////////////////////////

Boxshape::Boxshape() : Shape(BOX)
{

}

Boxshape::~Boxshape()
{

}

Aabb	&Boxshape::compute_aabb(Aabb &aabb, vec<float, 4> const &position) const
{
	aabb.bottom = position;
	aabb.top = position;
	return (aabb);
}

///////////////////////////////////////

Meshshape::Meshshape() : Shape(MESH)
{

}

Meshshape::~Meshshape()
{

}

Aabb	&Meshshape::compute_aabb(Aabb &aabb, vec<float, 4> const &position) const
{
	aabb.bottom = position;
	aabb.top = position;
	return (aabb);
}

///////////////////////////////////////////////////////////////////////////////
/*
float					sphere_sphere(Body const *x, Body const *y)
{
	vec<float, 4> const	p = x->position - y->posistion;
	vec<float, 4> const	v = x->velocity - y->velocity;

	float const	r = x->shape->radius + y->shape->radius;
    float const pp = vsqrnorm(p) - r * r;

	if (p <= 0.0)
		return (0.0f);

    float const	pv = vdot(p, v);

    if (pv >= 0.0)
        return (-1.0f);
	float const	vv = vsqrnorm(v);

	if ((pv + vv) <= 0.0f && (vv + 2.0f * pv + pp) >= 0.0f) //beware
		return (-1.0f);

	return (pp + -pv * (pv / vv));
}

float					sphere_aabox(Body const *x, Body const *y)
{
	return (-1.0f);
}

float					aabox_aabox(Body const *x, Body const *y)
{
	vec<float, 4> const	u = (y->position - (x->position + x->shape->size)) / (x->velocity - y->velocity);
	vec<float, 4> const	v = (x->position - (y->position + y->shape->size)) / (y->velocity - x->velocity);
	
	float const	tu = max(max(u[0], u[1]), u[2]);
	float const	tv = max(max(v[0], v[1]), v[2]);

	return (min(tu, tv));
}
*/