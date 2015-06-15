#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "random/mt19937.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"
/*
#include "projectile.hpp"
#include "player.hpp"
#include "bonus.hpp"
#include "feeder.hpp"
#include "level.hpp"*/
#include "acontroller.hpp"

FACTORYREG(Player);

Player::Player(Actormanager *a, Replication *r, int i, short int t, Actor const *o) : Actor(a, r, i, t, o), tolerance(1), dose(0), high(false), dmg(1.0f), firerate(2.0f), score(0.0f), loadingtime(0.0f)
{
	am->pe->add(&bdb, 0, true);
	
	firing = false;
	loadingfire = false;
	bdb->loc[0] = 128.0f;
	bdb->loc[1] = 128.0f;
	bdb->size = 25.0f;
	
}

Player::~Player()
{

}

void	Player::postinstanciation()
{
	if (am->graphic)
	{
		ps = new Particlesystem(am->ge, 1.0f, "player", &bdb);
		am->ge->add(ps);
		//hud = new Hud(1.0f, &cleared, this);
	}
	if (!am->master)
		am->notify_owner(this, true);

}

void	Player::destroy()
{
	Actor::destroy();
	am->pe->remove(bdb);
	if (am->graphic)
		am->ge->remove(ps);
}

void	Player::notified_by_owner(Actor *a, bool)
{
	AController	*c = (AController *)a;

	c->controlled = this;
	if (am->graphic && am->_controllermap.find(c->id) != am->_controllermap.end())
	{
		am->ge->cam_pos = bdb->loc;
		//am->ge->add(hud);
	}
}

void	Player::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	pckt.write(dir);
	pckt.write(firing);
	bdb->get_replication(pckt);
}

void	Player::replicate(Packet &pckt, float p)
{
	Actor::replicate(pckt, p);
	pckt.read(dir);
	pckt.read(firing);
	bdb->replicate(pckt, p);
}

void	Player::tick(float delta)
{
	Actor::tick(delta);

	if (am->master)
	{
		if (loadingfire)
		{
			loadingtime += delta;
			firing = false;
		}
		else if (loadingtime > 0.0f)
		{
/*			float	a = loadingtime * pow(log(firerate + M_E), 3);
			for (float i = 0.0f; i < a; ++i)
			{
				vec<float, 2>	a;
				Projectile		*p = (Projectile *)am->create("Projectile", this);

				p->ownerid = id;
				p->dmg = dmg;
				p->bd.loc = bd.loc;
				a[0] = MT().genrand_real1(-1.0, 1.0);
				a[1] = MT().genrand_real1(-1.0, 1.0);
				p->bdb->spd = Sgl::unit(a) * 512.0f;
			}
			loadingtime = 0.0f;*/
		}
		else if (firing && !is_callback_started("fire"))
		{
			fire();
			start_callback("fire", 1.0f / firerate, true, (bool (Actor::*)())&Player::fire);
		}
	}
}

bool			Player::collide(Actor const &x)
{
/*	Bonus const	*b;
	Feeder const *f;

	if ((b = dynamic_cast<Bonus const *>(&x)))
	{
		dmg += b->dmg;
		firerate += b->firerate;
		update_callback("fire", 1.0f / firerate);
	}
	else if ((f = dynamic_cast<Feeder const *>(&x)))
	{
		if (am->graphic)
			am->ge->add(new Particlesystem(0.0f, "damage", bd.loc));
		dose += 1;
		start_callback(3.0f, (bool (Actor::*)())&Player::down);
		if (!high && dose == tolerance)
			high = true;
		if (dose > tolerance)
			;//am->em.running = false;
	}
	else if (dynamic_cast<Level const *>(&x))
		return (true);*/
	return (false);
}

bool	Player::fire()
{
	/*if (firing)
	{
		if (am->master)
		{
			Projectile *p = (Projectile *)am->create("Projectile", this);
			p->ownerid = id;
			p->dmg = dmg;
			p->bd.loc = bd.loc;
			p->bd.spd = dir * 512.0f;
		}
		return (true);
	}
	else
		return (false);*/
	return (false);
}

bool	Player::down()
{
	if (!--dose && high)
	{
		high = false;
		++tolerance;
	}
	return (true);
}
