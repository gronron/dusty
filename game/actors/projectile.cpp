#include <cmath>
#include "endian/packet.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "gameengine.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"
/*
#include "feeder.hpp"
#include "level.hpp"*/
#include "projectile.hpp"

FACTORYREG(Projectile);

Projectile::Projectile(Gameengine *g, Replication *r, int const i, short int const t, Actor const *o) : Actor(g, r, i, t, o), body(0)
{
	dmg = 1.0f;
	dir = 0.0f;
	engine->physic->new_body(&body, &shape, this);
	shape.radius = 20.0f;
	body->dynamic = true;
}

Projectile::~Projectile()
{
	engine->physic->delete_body(body);
}

void	Projectile::postinstanciation()
{
	Actor::postinstanciation();
	engine->physic->init_body(body);
	if (engine->graphic)
	{
		ps = new Particlesystem(engine->graphic, 1.0f, "projectile", &body);
		engine->graphic->add(ps);
	}
	start_callback("Destroy", 8.0f, false, (bool (Actor::*)())&Projectile::selfdestroy);
}

void	Projectile::destroy()
{
	Actor::destroy();
	if (engine->graphic)
		ps->stop();
}

void	Projectile::get_replication(Packet &pckt) const
{
	Actor::get_replication(pckt);
	pckt.write(dmg);
	pckt.write(dir);
	body->get_replication(pckt);
}

void	Projectile::replicate(Packet &pckt, float const p)
{
	Actor::replicate(pckt, p);
	pckt.read(dmg);
	pckt.read(dir);
	body->replicate(pckt, p);
}

bool	Projectile::collide(Collider *x)
{
	/*if (dynamic_cast<Feeder const *>(&x) || dynamic_cast<Level const *>(&x))
	{
		destroy();
		return (true);
	}*/
	return (false);
}

bool	Projectile::selfdestroy()
{
	destroy();
	return (true);
}
