#ifndef FEEDER_H_
#define FEEDER_H_

#include "actor.hpp"
#include "particlesystem.hpp"

class	Feeder : public Actor
{
	public:

		Body			*body;
		Sphereshape		shape;

		float			hp;

		vec<float, 2>	aim;
		vec<float, 2>	dir;

		Particlesystem	*ps;


		Feeder(Actormanager *, Replication *, int, short int, Actor const *);
		virtual ~Feeder();

		void	postinstanciation();
		void	destroy();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	collide(Collider *);

		bool	targetsomeone();
};

#endif
