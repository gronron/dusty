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

#include <cfloat>
#include "new.hpp"
#include "math/vec_util.hpp"
#include "shape.hpp"
#include "physicengine.hpp"

struct	Querywrapper
{
	Physicengine const	*physic;
	Querycallback		*qcb;

	void	callback(int const aabbidx, int const bdidx)
	{
		qcb->report_encounter(physic->_bodies + bdidx, aabbidx);
	}
};

struct	Sphericalquerywrapper
{
	Physicengine const	*physic;
	Querycallback		*qcb;
	vec<float, 4>		center;
	float				radius;

	void	dcallback(int const aabbidx, int const bdidx)
	{
		//check if aabb is in
		qcb->report_encounter(physic->_bodies + bdidx, aabbidx);
	}
	
	void	scallback(int const aabbidx, int const bdidx)
	{
		if (intersect_sphereaabb(center, radius, physic->_statictree._nodes[aabbidx].aabb))
			qcb->report_encounter(physic->_bodies + bdidx, aabbidx);
	}
};

struct	Raycastwrapper
{
	Physicengine const	*physic;
	Raycastcallback		*rcb;

	bool	callback(int const aabbidx, int const bdidx, float const near, float const far)
	{
		return (rcb->report_encounter(physic->_bodies + bdidx, aabbidx, near, far));
	}
};

void				Physicengine::query(Aabb const &aabb, Querycallback *qcb, bool const d, bool const s) const
{
	Querywrapper	qwp = { this, qcb };

	if (d)
		_dynamictree.query(aabb, &qwp, &Querywrapper::callback);
	if (s)
		_statictree.query(aabb, &qwp, &Querywrapper::callback);
}

void						Physicengine::query(vec<float, 4> const &center, float const radius, Querycallback *qcb, bool const d, bool const s) const
{
	Sphericalquerywrapper	sqwp = { this, qcb, center, radius };
	Aabb					aabb;

	aabb.bottom = center - radius;
	aabb.top = center + radius;

	if (d)
		_dynamictree.query(aabb, &sqwp, &Sphericalquerywrapper::dcallback);
	if (s)
		_statictree.query(aabb, &sqwp, &Sphericalquerywrapper::scallback);
}

void				Physicengine::raycast_through(Ray const &ray, Raycastcallback *rcb, bool const d, bool const s) const
{
	Raycastwrapper	rwp = { this, rcb };

	if (d)
		_dynamictree.raycast_through(ray, &rwp, &Raycastwrapper::callback);
	if (s)
		_statictree.raycast_through(ray, &rwp, &Raycastwrapper::callback);
}

void				Physicengine::raycast(Ray const &ray, Raycastcallback *rcb, bool const d, bool const s) const
{
	Raycastwrapper	rwp = { this, rcb };

	if (d)
		_dynamictree.raycast(ray, &rwp, &Raycastwrapper::callback);
	if (s)
		_statictree.raycast(ray, &rwp, &Raycastwrapper::callback);
}
