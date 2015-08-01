#include <cmath>
#include "endian/packet.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"
/*
#include "feeder.hpp"
#include "level.hpp"*/
#include "projectile.hpp"

FACTORYREG(Projectile);

Projectile::Projectile(Actormanager *a, Replication *r, int i, short int t, Actor const *o) : Actor(a, r, i, t, o), body(0)
{
	dmg = 1.0f;
	dir = 0.0f;
	am->pe->new_body(&body);
	body->collider = this;
	shape.radius = 20.0f;
	body->shape = &shape;
	body->dynamic = true;
}

Projectile::~Projectile()
{
	am->pe->delete_body(body);
}

void	Projectile::postinstanciation()
{
	Actor::postinstanciation();
	am->pe->init_body(body);
	if (am->graphic)
	{
		ps = new Particlesystem(am->ge, 1.0f, "projectile", &body);
		am->ge->add(ps);
	}
	start_callback("Destroy", 8.0f, false, (bool (Actor::*)())&Projectile::selfdestroy);
}

void	Projectile::destroy()
{
	Actor::destroy();
	if (am->graphic)
		ps->stop();
}

void	Projectile::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	pckt.write(dmg);
	pckt.write(dir);
	body->get_replication(pckt);
}

void	Projectile::replicate(Packet &pckt, float p)
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
