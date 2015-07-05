#ifndef LEVEL_H_
#define LEVEL_H_

#include <vector>
#include "actor.hpp"
#include "boundingbox.hpp"
#include "leveldisplayer.hpp"

class	Level : public Actor
{
	public:

		struct Block
		{
			int				t;
			vec<float, 2>	loc;
			Boundingbox		*bdb;
		};

		unsigned int		x;
		unsigned int		y;
		char				**map;

		std::vector<Block>	_map;

		Leveldisplayer	*ld;

		int		nbr;
		float	spawnrate;

		Level(Actormanager *, Replication *, int, short int, Actor const *);
		virtual ~Level();

		void	postinstanciation();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	spawn_feeder();
		bool	increase_difficulty();

		void	_generate_block();
};

#endif
