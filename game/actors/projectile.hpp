#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "actor.hpp"
#include "body.hpp"
#include "particlesystem.hpp"

class	Projectile : public Actor
{
	public:

		Body	bd;

		float	dmg;

		vec<float, 2>	dir;

		Particlesystem	*ps;


		Projectile(Actormanager *, Replication *, short int, int, Actor const *);
		virtual ~Projectile();

		void	postinstanciation();
		void	destroy();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	collide(Actor const &);

		bool	selfdestroy();
};

#endif
