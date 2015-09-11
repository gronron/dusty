#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "random/mt19937.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "gameengine.hpp"
#include "callbackmanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"
#include "projectile.hpp"
#include "player.hpp"
//#include "bonus.hpp"
#include "feeder.hpp"
#include "level.hpp"
#include "acontroller.hpp"

#include <iostream>
#include <cmath>
#define M_E 2.718281828459045

FACTORYREG(Player);

Player::Player(Gameengine *g, Replication *r, int const i, short int const t, Actor const *o) : Actor(g, r, i, t, o), dmg(1.0f), firerate(200.0f), score(0.0f), loadingtime(0.0f)
{
	engine->physic->new_body(&body, &shape, this);
	shape.radius = 32.0f;
	body->dynamic = true;
	body->position = 128.0f;
	
	firing = true;
	loadingfire = false;
}

Player::~Player()
{

}

void	Player::postinstanciation()
{
	Actor::postinstanciation();
	engine->physic->init_body(body);
	if (engine->graphic)
	{
		ps = new Particlesystem(engine->graphic, 1.0f, "player", &body);
		engine->graphic->add(ps);
		//hud = new Hud(1.0f, &cleared, this);
	}
	if (!engine->master)
		engine->notify_owner(this, true);

}

void	Player::destroy()
{
	Actor::destroy();
	engine->physic->delete_body(body);
	if (engine->graphic)
		engine->graphic->remove(ps);
}

void	Player::notified_by_owner(Actor *a, bool const)
{
	AController	*c = (AController *)a;

	c->controlled = this;
	if (engine->graphic && engine->_controllermap.find(c->id) != engine->_controllermap.end())
	{
		//engine->graphic->cam_pos = bdb->loc;
		//engine->graphic->add(hud);
	}
}

void	Player::get_replication(Packet &pckt) const
{
	Actor::get_replication(pckt);
	pckt.write(dir);
	pckt.write(firing);
	body->get_replication(pckt);
}

void	Player::replicate(Packet &pckt, float const p)
{
	Actor::replicate(pckt, p);
	pckt.read(dir);
	pckt.read(firing);
	body->replicate(pckt, p);
}

void	Player::tick(float const delta)
{
	Actor::tick(delta);
	if (engine->master)
	{
		if (loadingfire)
		{
			loadingtime += delta;
			firing = false;
		}
		else if (loadingtime > 0.0f)
		{
			float	a = loadingtime * (float)pow(log(firerate + M_E), 3);
			for (float i = 0.0f; i < a; ++i)
			{
				vec<float, 2>	a;
				Projectile		*p = (Projectile *)engine->create("Projectile", this, true);

				p->ownerid = id;
				p->dmg = dmg;
				p->body->position = body->position;
				a[0] = (float)MT().genrand_real1(-1.0, 1.0);
				a[1] = (float)MT().genrand_real1(-1.0, 1.0);
				p->body->velocity = unit<float>(a) * 512.0f;
			}
			loadingtime = 0.0f;
		}
		else if (firing && !engine->callback->is_callback_started(1, this))
		{
			fire();
			//engine->callback->start_callback(1, this, (bool (Actor::*)())&Player::fire, 1.0f / firerate, true);
		}
	}
}

bool			Player::collide(Collider *x)
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
		if (engine->graphic)
			engine->graphic->add(new Particlesystem(0.0f, "damage", bd.loc));
		dose += 1;
		start_callback(3.0f, (bool (Actor::*)())&Player::down);
		if (!high && dose == tolerance)
			high = true;
		if (dose > tolerance)
			;//engine->em.running = false;
	}*/
	if (dynamic_cast<Level const *>(x))
		return (true);
	return (false);
}

bool	Player::fire()
{
	if (firing)
	{
		if (engine->master)
		{
			Projectile *p = (Projectile *)engine->create("Projectile", this, true);
			p->ownerid = id;
			p->dmg = dmg;
			p->body->position = body->position;
			p->body->velocity = dir * 512.0f;
		}
		return (true);
	}
	else
		return (false);
	return (false);
}
