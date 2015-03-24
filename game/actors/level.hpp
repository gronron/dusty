#ifndef LEVEL_H_
#define LEVEL_H_

#include <vector>
#include "actor.hpp"
#include "body.hpp"
#include "leveldisplayer.hpp"

class	Level : public Actor
{
	public:

		struct Block
		{
			int				t;
			vec<float, 2>	loc;
			Body			*bd;
		};

		unsigned int		x;
		unsigned int		y;
		char				**map;

		std::vector<Block>	_map;

		Leveldisplayer	*ld;

		int		nbr;
		float	spawnrate;

		Level(Actormanager *, Replication *, short int, int, Actor const *);
		virtual ~Level();

		void	postinstanciation();

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		bool	spawn_feeder();
		bool	increase_difficulty();

		void	_generate_block();
};

#endif
