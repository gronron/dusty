#ifndef BONUS_H_
#define BONUS_H_

#include "actor.hpp"
#include "boundingbox.hpp"
#include "particlesystem.hpp"

class	Bonus : public Actor
{
	public:

		Boundingbox	*bdb;

		float	dmg;
		float	firerate;

		Particlesystem	*ps;

		Bonus(Actormanager *, Replication *, int, short int, Actor const *);
		virtual ~Bonus();

		void	postinstanciation();
		void	destroy();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	collide(Actor const &);

		bool	timeout();
};

#endif
