#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "actor.hpp"
#include "boundingbox.hpp"
#include "particlesystem.hpp"

class	Projectile : public Actor
{
	public:

		Boundingbox		*bdb;

		float	dmg;

		vec<float, 2>	dir;

		Particlesystem	*ps;


		Projectile(Actormanager *, Replication *, int, short int, Actor const *);
		virtual ~Projectile();

		void	postinstanciation();
		void	destroy();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	collide(Actor const &);

		bool	selfdestroy();
};

#endif
