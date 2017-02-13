/******************************************************************************
Copyright (c) 2015-2016, Geoffrey TOURON
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

#include "human_structure.hpp"

struct	Result : public Querycallback
{
	Body	*body;
	Actor	*actor;

	Result(Body *bd) : body(bd), actor(0) { }

	bool	report_encounter(Body *bd, int const)
	{
		Actor	*actr = dynamic_cast<Actor *>(bd->collider);

		if (actr && bd != body)
		{
			actor = actr;
			return (false);
		}
		return (true);
	}
}

bool	Medicalstation::finished()
{
	Structure::finished();
	engine->callback->start_callback(HAHS32("heal"), this, (bool (Entity::*)())&Medicalstation::heal, 0.5f, true);
}

bool	Medicalstation::heal()
{
	Aabb	aabb;
	vec<float, 4> const size = { 1.0f, 1.0f, 2.0f, 0.0f};
	
	aabb.bottom = body->position - shape.size * 0.5f;
	aabb.top = aabb.bottom + size;

	if (target)
	{
		if (!target->shape.compute_aabb(target->body->position).is_overlapping(aabb))
			target = 0;
	}
	
	if (!target)
	{
		Result	result(body);
		
		engine->physic->query(aabb, &result, true, false);
		if (result->actor)
		{
			target = result->actor;
		}
	}
	
	if (targetid >= 0)
	{
		Actor	*actr = (Actor *)engine->find_entity(targetid);
		
		actr->health += 10;
		if (actr->health > actr->maxhealth)
			actr->health = actr->maxhealth;
	}
}

#endif
