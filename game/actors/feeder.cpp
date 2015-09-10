#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "random/mt19937.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "callbackmanager.hpp"
#include "gameengine.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"

#include "player.hpp"
#include "projectile.hpp"
//#include "bonus.hpp"
#include "feeder.hpp"

FACTORYREG(Feeder);

Feeder::Feeder(Gameengine *g, Replication *r, int const i, short int const t, Actor const *o) : Actor(g, r, i, t, o), body(0), hp(2.0f)
{
	engine->physic->new_body(&body, &shape, this);
	shape.radius = 20.0f;
	body->dynamic = true;
	aim = 0.0f;
}

Feeder::~Feeder()
{
	engine->physic->delete_body(body);
}

void	Feeder::postinstanciation()
{
	Actor::postinstanciation();
	engine->physic->init_body(body);
	if (engine->graphic)
	{
		ps = new Particlesystem(engine->graphic, 0.0f, "feeder", &body);
		engine->graphic->add(ps);
	}
	engine->callback->start_callback(1, this, (bool (Actor::*)())&Feeder::targetsomeone, 0.3f, true);
}		

void	Feeder::destroy()
{
	Actor::destroy();
	if (engine->graphic)
	{
		ps->stop();
		engine->graphic->add(new Particlesystem(engine->graphic, 0.0f, "explosion", (vec<float, 3>)body->position));
	}
}

void	Feeder::get_replication(Packet &pckt) const
{
	Actor::get_replication(pckt);
	pckt.write(aim);
	pckt.write(hp);
	body->get_replication(pckt);
}

void	Feeder::replicate(Packet &pckt, float const p)
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
	for (unsigned int i = 0; i < engine->_actsize; ++i)
	{
		if (engine->_actors[i] && (plr = dynamic_cast<Player *>(engine->_actors[i])))
		{
			cur = norm<float>(body->position - plr->body->position);
			if (cur > max)
			{
				max = cur;
				aim = plr->body->position;
			}
		}
	}
	dir = unit<float>(aim - body->position);
	body->velocity = dir * 150.0f;
	return (true);
}

bool	Feeder::collide(Collider *x)
{
	Projectile const	*prj;
	//Bonus				*b;

	destroy();
	if ((prj = dynamic_cast<Projectile const *>(x)))
	{
		if (engine->master && rp)
			rp->needupdate = true;
		if ((hp -= prj->dmg) <= 0.0f)
		{
			/*if (engine->master && !(MT().genrand_int32() % 7))
			{
				b = (Bonus *)engine->create("Bonus", 0);
				b->bd.loc = bd.loc;
				b->dmg = 0.2f;
				b->firerate = 1.0f;
			}
			/*Player *plr = (Player *)engine->findactor(x.ownerid);
			if (plr)
				plr->score++;*/
			destroy();
		}
	}
	else if (dynamic_cast<Player const *>(x))
		destroy();
	return (false);
}
