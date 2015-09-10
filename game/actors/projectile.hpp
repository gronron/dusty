#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "actor.hpp"
#include "particlesystem.hpp"

class	Projectile : public Actor
{
	public:

		Body		*body;
		Sphereshape	shape;

		float	dmg;

		vec<float, 2>	dir;

		Particlesystem	*ps;


		Projectile(Gameengine *, Replication *, int const, short int const, Actor const *);
		virtual ~Projectile();

		void	postinstanciation();
		void	destroy();

		void	get_replication(Packet &) const;
		void	replicate(Packet &, float const);

		bool	collide(Collider *);

		bool	selfdestroy();
};

#endif
