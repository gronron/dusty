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

#include "df/df_node.hpp"
#include "random/mt19937.hpp"
#include "particle/effectfactory.hpp"
#include "commoneffect.hpp"

EFFECTFACTORYREG(Expulsor);

Expulsor::Expulsor(Particlesystem *p, float const t, Df_node const *d) : Particleeffect(p, t, d)
{
	velocity = *(vec<float, 2> *)d->safe_get("velocity", Df_node::FLOAT, 2)->flt;
}

Expulsor::~Expulsor()
{

}

void	Expulsor::tick(float const delta)
{
	for (unsigned int i = 0; i < ps->_prtclssize; ++i)
	{
		ps->_particles[i].position += ps->_particles[i].velocity * delta;

		if (!ps->_particles[i].active)
		{
			ps->_particles[i].position = ps->position;
			ps->_particles[i].velocity[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			ps->_particles[i].velocity[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			ps->_particles[i].velocity[2] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			ps->_particles[i].velocity = vunit<float>(ps->_particles[i].velocity) * (float)MT().genrand_real1(velocity[0], velocity[1]);
		}
	}
}

///////////////////////////////////////

EFFECTFACTORYREG(Attractor);

Attractor::Attractor(Particlesystem *p, float const t, Df_node const *d) : Particleeffect(p, t, d)
{
	velocity = *(vec<float, 2> *)d->safe_get("velocity", Df_node::FLOAT, 2)->flt;
}

Attractor::~Attractor()
{

}

void	Attractor::tick(float const delta)
{
	for (unsigned int i = 0; i < ps->_prtclssize; ++i)
	{
		ps->_particles[i].position += ps->_particles[i].velocity * delta;

		if (!ps->_particles[i].active)
		{
			ps->_particles[i].velocity[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f); 
			ps->_particles[i].velocity[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			ps->_particles[i].velocity[2] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			ps->_particles[i].velocity = vunit<float>(ps->_particles[i].velocity) * (float)MT().genrand_real1(velocity[0], velocity[1]);
			ps->_particles[i].position = ps->position - ps->_particles[i].velocity / ps->_particles[i].fade_rate;
		}
	}
}

///////////////////////////////////////

EFFECTFACTORYREG(Appear);

Appear::Appear(Particlesystem *p, float const t, Df_node const *d) : Particleeffect(p, t, d)
{
	if (t < 0.0f)
		fade_rate = *(vec<float, 2> *)d->safe_get("rate", Df_node::FLOAT, 2)->flt;
	else
		fade_rate = { 1.0f / t, 1.0f / t };
	
	for (unsigned int i = 0; i < ps->_prtclssize; ++i)
	{
		if (!ps->_particles[i].active && ps->running)
		{
			ps->_particles[i].size = 0.0;
			ps->_particles[i].fade_rate = (float)MT().genrand_real1(fade_rate[0], fade_rate[1]);
		}
	}
}

Appear::~Appear()
{

}

void	Appear::tick(float const delta)
{
	for (unsigned int i = 0; i < ps->_prtclssize; ++i)
	{
		ps->_particles[i].size += ps->_particles[i].fade_rate * delta;
		if (!ps->_particles[i].active && ps->running)
			ps->_particles[i].active = true;
		else if (ps->_particles[i].size >= 1.0f)
		{
			ps->_particles[i].active = false;
			ps->_particles[i].size = 0.0f;
			ps->_particles[i].fade_rate =  (float)MT().genrand_real1(fade_rate[0], fade_rate[1]);
		}
	}
}

///////////////////////////////////////

EFFECTFACTORYREG(Disappear);

Disappear::Disappear(Particlesystem *p, float const t, Df_node const *d) : Particleeffect(p, t, d)
{
	if (t < 0.0f)
		fade_rate = *(vec<float, 2> *)d->safe_get("rate", Df_node::FLOAT, 2)->flt;
	else
		fade_rate = { 1.0f / t, 1.0f / t };
	for (unsigned int i = 0; i < ps->_prtclssize; ++i)
	{
		if (!ps->_particles[i].active && ps->running)
		{
			ps->_particles[i].size = (float)MT().genrand_real1(0.0, 1.0);
			ps->_particles[i].fade_rate = (float)MT().genrand_real1(fade_rate[0], fade_rate[1]);
		}
	}
}

Disappear::~Disappear()
{

}

void	Disappear::tick(float const delta)
{
	for (unsigned int i = 0; i < ps->_prtclssize; ++i)
	{
		ps->_particles[i].size -= ps->_particles[i].fade_rate * delta;
		if (!ps->_particles[i].active && ps->running)
			ps->_particles[i].active = true;
		else if (ps->_particles[i].size <= 0.0f)
		{
			ps->_particles[i].active = false;
			ps->_particles[i].size = 1.0f;
			ps->_particles[i].fade_rate = (float)MT().genrand_real1(fade_rate[0], fade_rate[1]);
		}
	}
}
