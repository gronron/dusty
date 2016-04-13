#include "common.hpp"
#include "world.hpp"
#include "projectile.hpp"

FACTORYREG(Projectile);

Projectile::Projectile(Gameengine *g, Replication *r, int const i, short int const t, Entity const *o) : Entity(g, r, i, t, o), body(0)
{
	shape.size = 0.01f;
	engine->physic->new_body(&body, &shape, this);
	
	body->dynamic = true;
	body->elasticity = 0.8f;
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
	Entity::postinstanciation();
	engine->physic->init_body(body);
	if (engine->graphic)
	{
		ps = new Particlesystem(engine->graphic, "projectile", &body);
		engine->graphic->add_animation(ps);
	}
	engine->callback->start_callback(4, this, (bool (Entity::*)())&Projectile::selfdestroy, 16.0f, false);
}

void	Projectile::destroy()
{
	Entity::destroy();
	if (engine->graphic)
		ps->stop();
}

void	Projectile::get_replication(Packet &pckt) const
{
	Entity::get_replication(pckt);
	pckt.write(damage);
	body->get_replication(pckt);
}

void	Projectile::replicate(Packet &pckt, float const p)
{
	Entity::replicate(pckt, p);
	pckt.read(damage);
	body->replicate(pckt, p);
}

bool	Projectile::should_collide(Collider const *x) const
{
	if (dynamic_cast<World const *>(x))
	{
		return (true);
	}
	return (false);
}

bool	Projectile::collide(Collider *x)
{
	if (dynamic_cast<World const *>(x))
	{
		engine->graphic->add_animation(new Particlesystem(engine->graphic, "explosion", body->position));
		//destroy();
		return (true);
	}
	return (false);
}

bool	Projectile::selfdestroy()
{
	destroy();
	return (true);
}
