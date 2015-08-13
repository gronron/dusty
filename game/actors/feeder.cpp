#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "random/mt19937.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"

#include "player.hpp"
#include "projectile.hpp"
//#include "bonus.hpp"
#include "feeder.hpp"

FACTORYREG(Feeder);

Feeder::Feeder(Actormanager *a, Replication *r, int i, short int t, Actor const *o) : Actor(a, r, i, t, o), body(0), hp(2.0f)
{
	am->pe->new_body(&body, &shape, this);
	shape.radius = 20.0f;
	body->dynamic = true;
	aim = 0.0f;
}

Feeder::~Feeder()
{
	am->pe->delete_body(body);
}

void	Feeder::postinstanciation()
{
	Actor::postinstanciation();
	am->pe->init_body(body);
	if (am->graphic)
	{
		ps = new Particlesystem(am->ge, 0.0f, "feeder", &body);
		am->ge->add(ps);
	}
	start_callback("TP", 0.3f, true, (bool (Actor::*)())&Feeder::targetsomeone);
}		

void	Feeder::destroy()
{
	Actor::destroy();
	if (am->graphic)
	{
		ps->stop();
		am->ge->add(new Particlesystem(am->ge, 0.0f, "explosion", (vec<float, 3>)body->position));
	}
}

void	Feeder::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	pckt.write(aim);
	pckt.write(hp);
	body->get_replication(pckt);
}

void	Feeder::replicate(Packet &pckt, float p)
{
	Actor::replicate(pckt, p);
	pckt.read(aim);
	pckt.read(hp);
	body->replicate(pckt, p);
}

bool	Feeder::targetsomeone()
{
	Player	*plr;
	float	max;
	float	cur;

	aim = 0.0f;
	max = 0.0f;
	cur = 0.0f;
	for (unsigned int i = 0; i < am->_asize; ++i)
	{
		if (am->_actors[i] && (plr = dynamic_cast<Player *>(am->_actors[i])))
		{
			cur = Sgl::norm(body->position - plr->body->position);
			if (cur > max)
			{
				max = cur;
				aim = plr->body->position;
			}
		}
	}
	dir = Sgl::unit(aim - body->position);
	body->velocity = dir * 150.0f;
	return (true);
}

#include <iostream>

bool	Feeder::collide(Collider *x)
{
	Projectile const	*prj;
	//Bonus				*b;

	destroy();
	if ((prj = dynamic_cast<Projectile const *>(x)))
	{
		if (am->master && rp)
			rp->needupdate = true;
		if ((hp -= prj->dmg) <= 0.0f)
		{
			/*if (am->master && !(MT().genrand_int32() % 7))
			{
				b = (Bonus *)am->create("Bonus", 0);
				b->bd.loc = bd.loc;
				b->dmg = 0.2f;
				b->firerate = 1.0f;
			}
			/*Player *plr = (Player *)am->findactor(x.ownerid);
			if (plr)
				plr->score++;*/
			destroy();
		}
	}
	else if (dynamic_cast<Player const *>(x))
		destroy();
	return (false);
}
