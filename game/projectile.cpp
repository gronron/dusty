#include "common.hpp"
#include "world.hpp"
#include "projectile.hpp"

FACTORYREG(Projectile);

Projectile::Projectile(Gameengine *g, Replication *r, int const i, short int const t, Actor const *o) : Actor(g, r, i, t, o), body(0)
{
	shape.size = 0.1f;
	engine->physic->new_body(&body, &shape, this);
	
	body->dynamic = true;
	damage = 1.0f;
}

Projectile::~Projectile()
{
	engine->physic->delete_body(body);
	engine->graphic->remove_animation(ps);
	delete ps;
}

void	Projectile::postinstanciation()
{
	Actor::postinstanciation();
	engine->physic->init_body(body);
	if (engine->graphic)
	{
		ps = new Particlesystem(engine->graphic, "projectile", &body);
		engine->graphic->add_animation(ps);
	}
	engine->callback->start_callback(1, this, (bool (Actor::*)())&Projectile::selfdestroy, 8.0f, false);
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
	pckt.write(damage);
	body->get_replication(pckt);
}

void	Projectile::replicate(Packet &pckt, float const p)
{
	Actor::replicate(pckt, p);
	pckt.read(damage);
	body->replicate(pckt, p);
}

bool	Projectile::collide(Collider *x)
{
	if (dynamic_cast<World const *>(x))
	{
		engine->graphic->add_animation(new Particlesystem(engine->graphic, "player", body->position));
		destroy();
		return (true);
	}
	return (false);
}

bool	Projectile::selfdestroy()
{
	destroy();
	return (true);
}
