#ifndef LEVEL_H_
#define LEVEL_H_

#include "actor.hpp"
#include "body.hpp"
#include "leveldisplayer.hpp"

class	Level : public Actor
{
	public:

	
		unsigned int		x;
		unsigned int		y;
		char				**map;
		Body				***bodies;

		Axisboxshape	shape;
		Leveldisplayer	*ld;

		int		nbr;
		float	spawnrate;

		Level(Gameengine *, Replication *, int const, short int const, Actor const *);
		virtual ~Level();

		void	postinstanciation();

		void	get_replication(Packet &) const;
		void	replicate(Packet &, float const);

		bool	spawn_feeder();
		bool	increase_difficulty();

		void	_generate_block();
};

#endif
