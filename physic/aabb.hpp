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

#ifndef AABB_H_
#define AABB_H_

#include "math/vec.hpp"
#include "math/vec_util.hpp"

struct				Ray
{
	vec<float, 4>	origin;
	vec<float, 4>	direction;
};

struct				Aabb
{
	vec<float, 4>	bottom;
	vec<float, 4>	top;
	
	Aabb	&merge(Aabb const &, Aabb const &);
	
	bool	is_overlapping(Aabb const &x) const;
	bool	is_containing(Aabb const &inner) const;
};

inline Aabb	&Aabb::merge(Aabb const &x, Aabb const &y)
{
	bottom = vmin(x.bottom, y.bottom);
	top = vmax(x.top, y.top);
	return (*this);
}

inline bool	Aabb::is_overlapping(Aabb const &x) const
{
	return ((vec<float, 3>)bottom <= x.top && (vec<float, 3>)top >= x.bottom);
}

inline bool	Aabb::is_containing(Aabb const &inner) const
{
	return ((vec<float, 3>)bottom <= inner.bottom && (vec<float, 3>)top >= inner.top);
}

inline	bool	intersect_rayaabb(Ray const &ray, Aabb const &aabb, float &tnear, float &tfar)
{
	vec<float, 3> const	invdir = 1.0f / ray.direction;
	vec<float, 3> const	tbot = (aabb.bottom - ray.origin) * ray.direction;
	vec<float, 3> const	ttop = (aabb.top - ray.origin) * ray.direction;
	vec<float, 3> const	tmin = vmin(tbot, ttop);
	vec<float, 3> const	tmax = vmax(tbot, ttop);

	tnear = max(max(tmin[0], tmin[1]), tmin[2]);
	tfar = min(min(tmax[0], tmax[1]), tmax[2]);

	return (tfar >= 0 && tnear <= tfar);
}

inline	bool	intersect_invrayaabb(Ray const &ray, Aabb const &aabb, float &tnear, float &tfar)
{
	vec<float, 3> const	tbot = (aabb.bottom - ray.origin) * ray.direction;
	vec<float, 3> const	ttop = (aabb.top - ray.origin) * ray.direction;
	vec<float, 3> const	tmin = vmin(tbot, ttop);
	vec<float, 3> const	tmax = vmax(tbot, ttop);

	tnear = max(max(tmin[0], tmin[1]), tmin[2]);
	tfar = min(min(tmax[0], tmax[1]), tmax[2]);

	return (tfar >= 0 && tnear <= tfar);
}

inline	bool	intersect_rayaabb_n(Ray const &ray, Aabb const &aabb, float &tnear, float &tfar, vec<float, 4> &normal)
{
	vec<float, 3> const	invdir = 1.0f / ray.direction;
	vec<float, 3> const	tbot = (aabb.bottom - ray.origin) * ray.direction;
	vec<float, 3> const	ttop = (aabb.top - ray.origin) * ray.direction;
	vec<float, 3> const	tmin = vmin(tbot, ttop);
	vec<float, 3> const	tmax = vmax(tbot, ttop);
	
	if (tmin[0] > tmin[1])
	{
		if (tmin[1] > tmin[2])
			normal = { invdir[0] >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f, 0.0f };
		else
			normal = { 0.0f, 0.0f, invdir[2] >= 0.0f ? -1.0f : 1.0f, 0.0f };
	}
	else
	{
		if (tmin[1] > tmin[2])
			normal = { 0.0f, invdir[1] >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f };
		else
			normal = { 0.0f, 0.0f, invdir[2] >= 0.0f ? -1.0f : 1.0f, 0.0f };
	}

	tnear = max(max(tmin[0], tmin[1]), tmin[2]);
	tfar = min(min(tmax[0], tmax[1]), tmax[2]);

	return (tfar >= 0 && tnear <= tfar);
}

inline	bool	intersect_invrayaabb_n(Ray const &ray, Aabb const &aabb, float &tnear, float &tfar, vec<float, 4> &normal)
{
	vec<float, 3> const	tbot = (aabb.bottom - ray.origin) * ray.direction;
	vec<float, 3> const	ttop = (aabb.top - ray.origin) * ray.direction;
	vec<float, 3> const	tmin = vmin(tbot, ttop);
	vec<float, 3> const	tmax = vmax(tbot, ttop);
	
	if (tmin[0] > tmin[1])
	{
		if (tmin[1] > tmin[2])
			normal = { ray.direction[0] >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f, 0.0f };
		else
			normal = { 0.0f, 0.0f, ray.direction[2] >= 0.0f ? -1.0f : 1.0f, 0.0f };
	}
	else
	{
		if (tmin[1] > tmin[2])
			normal = { 0.0f, ray.direction[1] >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f };
		else
			normal = { 0.0f, 0.0f, ray.direction[2] >= 0.0f ? -1.0f : 1.0f, 0.0f };
	}

	tnear = max(max(tmin[0], tmin[1]), tmin[2]);
	tfar = min(min(tmax[0], tmax[1]), tmax[2]);

	return (tfar >= 0 && tnear <= tfar);
}

#endif
