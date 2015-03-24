#include <cmath>
#include "endian/packet.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"

#include "feeder.hpp"
#include "level.hpp"
#include "projectile.hpp"

FACTORYREG(Projectile);

Projectile::Projectile(Actormanager *a, Replication *r, short int t, int i, Actor const *o) : Actor(a, r, t, i, o), bd(this, true, true)
{
	dmg = 1.0f;
	dir = 0.0f;
	if (!am->local && !rp)
		rp = new Replication(this, 0.5f, 5.0f);
	bd.size = 20.0f;
	am->pe->add(&bd);
}

Projectile::~Projectile()
{
	am->pe->remove(&bd);
}

void	Projectile::postinstanciation()
{
	if (am->graphic)
	{
		ps = new Particlesystem(0.0f, &cleared, "projectile", &bd);
		am->ge->add(ps);
	}
	start_callback("Destroy", 8.0f, false, (bool (Actor::*)())&Projectile::selfdestroy);
}

void	Projectile::destroy()
{
	Actor::destroy();
	if (am->graphic)
		am->ge->call(ps, (void (Animation::*)())&Particlesystem::stop);
}

void	Projectile::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	pckt.write(dmg);
	pckt.write(dir);
	bd.get_replication(pckt);
}

void	Projectile::replicate(Packet &pckt, float p)
{
	Actor::replicate(pckt, p);
	pckt.read(dmg);
	pckt.read(dir);
	bd.replicate(pckt, p);
}

bool	Projectile::collide(Actor const &x)
{
	if (dynamic_cast<Feeder const *>(&x) || dynamic_cast<Level const *>(&x))
	{
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
