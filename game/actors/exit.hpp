#ifndef EXIT_H_
#define EXIT_H_

#include "actor.hpp"
#include "body.hpp"
#include "particlesystem.hpp"

class	Exit : public Actor
{
	public:

		Body	bd;

		Particlesystem	*ps;

		Exit(Actormanager *, Replication *, short int, int, Actor const *);
		virtual ~Exit();

		void	postinstanciation();
		void	destroy();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	collide(Actor const &);
};

#endif
