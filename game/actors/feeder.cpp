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

Feeder::Feeder(Actormanager *a, Replication *r, int i, short int t, Actor const *o) : Actor(a, r, i, t, o), bdb(), hp(2.0f)
{
	am->pe->add(&bdb, 0, true);
	bdb->actor = this;
	bdb->size = 20.0f;
	aim = 0.0f;
}

Feeder::~Feeder()
{
	am->pe->remove(bdb);
}

void	Feeder::postinstanciation()
{
	if (am->graphic)
	{
		ps = new Particlesystem(am->ge, 0.0f, "feeder", &bdb);
		am->ge->add(ps);
	}
	start_callback("TP", 0.3f, true, (bool (Actor::*)())&Feeder::targetsomeone);
}		

void	Feeder::destroy()
{
	Actor::destroy();
	if (am->graphic)
	{
		am->ge->add(new Particlesystem(am->ge, 0.0f, "explosion", bdb->loc));
	}
}

void	Feeder::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	pckt.write(aim);
	pckt.write(hp);
	bdb->get_replication(pckt);
}

void	Feeder::replicate(Packet &pckt, float p)
{
	Actor::replicate(pckt, p);
	pckt.read(aim);
	pckt.read(hp);
	bdb->replicate(pckt, p);
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
			cur = Sgl::norm(bdb->loc - plr->bdb->loc);
			if (cur > max)
			{
				max = cur;
				aim = plr->bdb->loc;
			}
		}
	}
	dir = Sgl::unit(aim - bdb->loc);
	bdb->nextspd = dir * 150.0f;
	return (true);
}

bool	Feeder::collide(Actor const &x)
{
	Projectile const	*prj;
	//Bonus				*b;

	if ((prj = dynamic_cast<Projectile const *>(&x)))
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
	else if (dynamic_cast<Player const *>(&x))
		destroy();
	return (false);
}
