#include "endian/packet.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"

#include "player.hpp"
#include "bonus.hpp"

FACTORYREG(Bonus);

Bonus::Bonus(Actormanager *a, Replication *r, short int t, int i, Actor const *o) : Actor(a, r, t, i, o), bd(this, false, true)
{
	if (!am->local && !rp)
		rp = new Replication(this, 0.5f, 5.0f);
	bd.size = 32.0f;
	am->pe->add(&bd);
}

Bonus::~Bonus()
{
	am->pe->remove(&bd);
}

void	Bonus::postinstanciation()
{
	if (am->graphic)
	{
		ps = new Particlesystem(0.0f, &cleared, "bonus", &bd);
		am->ge->add(ps);
	}
	if (am->audio)
		Sound::manager.load("bonus_spawn.mp3", true)->play();
	start_callback(15.0f, (bool (Actor::*)())&Bonus::timeout);
}

void	Bonus::destroy()
{
	Actor::destroy();
	if (am->graphic)
	{
		am->ge->call(ps, (void (Animation::*)())&Particlesystem::reverse);
		am->ge->call(ps, (void (Animation::*)())&Particlesystem::stop);
	}
}

void	Bonus::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	pckt.write(dmg);
	pckt.write(firerate);
	bd.get_replication(pckt);
}

void	Bonus::replicate(Packet &pckt, float p)
{
	Actor::replicate(pckt, p);
	pckt.read(dmg);
	pckt.read(firerate);
	bd.replicate(pckt, p);
}

bool	Bonus::collide(Actor const &x)
{
	if (dynamic_cast<Player const *>(&x))
	{
		destroy();
		return (true);
	}
	return (false);
}

bool	Bonus::timeout()
{
	destroy();
	return (true);
}
