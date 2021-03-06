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

#ifndef SHAPE_H_
#define SHAPE_H_

#include "math/vec.hpp"
#include "aabb.hpp"

class	Shape
{
	public:

		enum Type	{ SPHERE, AXIS_CYLINDER, AXIS_ALIGNED_BOX, BOX, MESH };


		char	type;


		Shape(char const);
		virtual ~Shape();
		
		virtual Aabb	&compute_aabb(Aabb &aabb, vec<float, 4> const &position) const = 0;
};

class	Sphereshape : public Shape
{
	public:
	
		float	radius;


		Sphereshape();
		virtual ~Sphereshape();
		
		virtual Aabb	&compute_aabb(Aabb &aabb, vec<float, 4> const &position) const;
};

class	Axiscylindershape : public Shape
{
	public:
	
		char	axis;
		float	lenght;
		float	radius;


		Axiscylindershape();
		virtual ~Axiscylindershape();
		
		virtual Aabb	&compute_aabb(Aabb &aabb, vec<float, 4> const &position) const;
};

class	Axisalignedboxshape : public Shape
{
	public:
	
		vec<float, 4>	size;


		Axisalignedboxshape();
		virtual ~Axisalignedboxshape();
		
		virtual Aabb	&compute_aabb(Aabb &aabb, vec<float, 4> const &position) const;
};

class	Boxshape : public Shape
{
	public:
	
		vec<float, 4>	size;


		Boxshape();
		virtual ~Boxshape();
		
		virtual Aabb	&compute_aabb(Aabb &aabb, vec<float, 4> const &position) const;
};

class	Meshshape : public Shape
{
	public:

		unsigned int	vsize;
		unsigned int	tsize;
		float			*vertices;
		unsigned int	*triangles;


		Meshshape();
		virtual ~Meshshape();
		
		virtual Aabb	&compute_aabb(Aabb &aabb, vec<float, 4> const &position) const;
};

#endif
