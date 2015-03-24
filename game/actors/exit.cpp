#include "endian/packet.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"
#include "exit.hpp"

#include "player.hpp"

FACTORYREG(Exit);

Exit::Exit(Actormanager *a, Replication *r, short int t, int i, Actor const *o) : Actor(a, r, t, i, o), bd(this, true, true)
{
	if (!am->local && !rp)
		rp = new Replication(this, 0.5f, 5.0f);
	bd.size = 25.0f;
	am->pe->add(&bd);
}

Exit::~Exit()
{
	am->pe->remove(&bd);
}

void	Exit::postinstanciation()
{
	if (am->graphic)
	{
		ps = new Particlesystem(0.0f, &cleared, "exit", &bd);
		am->ge->add(ps);
	}
}

void	Exit::destroy()
{
	Actor::destroy();
	if (am->graphic)
		ps->destroyed = true;
}

void	Exit::get_replication(Packet &pckt)
{
	Actor::get_replication(pckt);
	bd.get_replication(pckt);
}

void	Exit::replicate(Packet &pckt, float p)
{
	Actor::replicate(pckt, p);
	bd.replicate(pckt, p);
}

bool	Exit::collide(Actor const &a)
{
	if (dynamic_cast<Player const *>(&a))
		;
	return (false);
}