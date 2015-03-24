#ifndef BOSS_H_
#define BOSS_H_

#include "actor.hpp"

class	Boss : public Actor
{
	public:

		Boss(Actormanager *, Replication *, short int, int, Actor const *);
		virtual ~Boss();

		virtual void	get_replication(Packet &);
        virtual void	replicate(Packet &, float);

		virtual void	tick(float);
};

#endif
