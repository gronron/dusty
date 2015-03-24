#ifndef BONUS_H_
#define BONUS_H_

#include "actor.hpp"
#include "body.hpp"
#include "particlesystem.hpp"

class	Bonus : public Actor
{
	public:

		Body	bd;

		float	dmg;
		float	firerate;

		Particlesystem	*ps;

		Bonus(Actormanager *, Replication *, short int, int, Actor const *);
		virtual ~Bonus();

		void	postinstanciation();
		void	destroy();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	collide(Actor const &);

		bool	timeout();
};

#endif
