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
#include "bonus.hpp"
#include "feeder.hpp"

FACTORYREG(Feeder);

Feeder::Feeder(Actormanager *a, Replication *r, short int t, int i, Actor const *o) : Actor(a, r, t, i, o), bd(this, true, true), hp(2.0f)
{
	if (!am->local && !rp)
		rp = new Replication(this, 0.5f, 5.0f);
	bd.loc[0] = 880.0f;
	bd.loc[1] = 70.0f;
	bd.size = 20.0f;
	aim = 0.0f;
	am->pe->add(&bd);
}

Feeder::~Feeder()
{
	am->pe->remove(&bd);
}

void	Feeder::postinstanciation()
{
	if (am->graphic)
	{
		ps = new Particlesystem(0.0f, &cleared, "feeder", &bd);
		am->ge->add(ps);
	}
	start_callback("TP", 0.3f, true, (bool (Actor::*)())&Feeder::targetsomeone);
}		

void	Feeder::destroy()
{
	Actor::destroy();
	if (am->graphic)
	{
		am->ge->call(ps, (void (Animation::*)())&Particlesystem::stop);
		am->ge->add(new Particlesystem(0.0f, "explosion", bd.loc));
	}
}

void	Feeder::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	pckt.write(aim);
	pckt.write(hp);
	bd.get_replication(pckt);
}

void	Feeder::replicate(Packet &pckt, float p)
{
	Actor::replicate(pckt, p);
	pckt.read(aim);
	pckt.read(hp);
	bd.replicate(pckt, p);
}

bool	Feeder::targetsomeone()
{
	Player	*plr;
	float	max;
	float	cur;

	aim = 0.0f;
	max = 0.0f;
	cur = 0.0f;
	for (Actormanager::Actoriterator i = am->_actormap.begin(); i != am->_actormap.end(); ++i)
	{
		if ((plr = dynamic_cast<Player *>(i->second)))
		{
			cur = Sgl::norm(bd.loc - plr->bd.loc);
			if (cur > max)
			{
				max = cur;
				aim = plr->bd.loc;
			}
		}
	}
	dir = Sgl::unit(aim - bd.loc);
	bd.spd = dir * 150.0f;
	return (true);
}

bool	Feeder::collide(Actor const &x)
{
	Projectile const	*prj;
	Bonus				*b;

	if ((prj = dynamic_cast<Projectile const *>(&x)))
	{
		if (am->master && rp)
			rp->needupdate = true;
		if ((hp -= prj->dmg) <= 0.0f)
		{
			if (am->master && !(MT().genrand_int32() % 7))
			{
				b = (Bonus *)am->create("Bonus", 0);
				b->bd.loc = bd.loc;
				b->dmg = 0.2f;
				b->firerate = 1.0f;
			}
			Player *plr = (Player *)am->findactor(x.ownerid);
			if (plr)
				plr->score++;
			destroy();
		}
	}
	else if (dynamic_cast<Player const *>(&x))
		destroy();
	return (false);
}
