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

#include "common.hpp"
#include "structure.hpp"

Structure::Structure(Gameengine *g, Replication *r, int const i, short int const t, Entity const *o) : Entity(g, r, i, t, o), body(0), team(0), avancement(0.0f), buildtime(6.0f), hp(0), maxhp(0)
{

}

Structure::~Structure()
{

}

Structure::post_instanciation()
{
	Actor::postinstanciation();
	
	Team	*team;
	if ((team = dynamic_cast<Team *>engine->find_entity(ownerid)))
		teamid = team->teamid;

	health = maxhealth * 0.1f;
	engine->callback->start_callback(HASH32("finished"), this, (bool (Entity::*)())&Structure::build, 0.1f, true);
	//launch building
}

Structure::destroy()
{
	
}

bool	Structure::build()
{
	if ((avancement += 0.1f) >= buildtime)
	{
		avancement = buildtime;
		ready = true;
	}
	health += (maxhealth - (maxhealth - buildtime * 0.1f)) / (buildtime * 10.0f);
	if (health > maxhealth)
		health = maxhealth;
}
