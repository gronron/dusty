#include "new.hpp"
#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "random/mt19937.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"

#include "labyrinth.hpp"
#include "player.hpp"
#include "feeder.hpp"
#include "level.hpp"

#include <iostream>

FACTORYREG(Level);

Level::Level(Actormanager *a, Replication *r, int i, short int t, Actor const *o) : Actor(a, r, i, t, o), nbr(0)
{
	if (am->master)
	{
		x = 16 * 4 + 1;
		y = x;
		map = generate_labyrinth(16, 16);
		_generate_block();
		spawnrate = 1.0f;
		start_callback("spawn_feeder", 1.0f / spawnrate, true, (bool (Actor::*)())&Level::spawn_feeder);
		start_callback("increase_difficulty", 8.0f, true, (bool (Actor::*)())&Level::increase_difficulty);
	}
}

Level::~Level()
{
	std::cout << "start ~level" << std::endl;
	delete_matrix(map);
	for (unsigned int i = 0; i < _map.size(); ++i)
	{
		if (_map[i].bdb)
			am->pe->remove(_map[i].bdb);
	}
	if (am->graphic)
		am->ge->remove(ld);
	std::cout << "done ~level" << std::endl;
}

void	Level::postinstanciation()
{
	if (!am->master)
		_generate_block();
	if (am->graphic)
	{
		ld = new Leveldisplayer(am->ge, -1.0f, this);
		am->ge->add(ld);
	}
}

void	Level::get_replication(Packet &pckt)
{
	pckt.write(ownerid);
	pckt.write(x);
	pckt.write(y);
	pckt.write(x * y, &map[0][0]);
}

void	Level::replicate(Packet &pckt, float p)
{
	ping = p;
	pckt.read(ownerid);
	pckt.read(x);
	pckt.read(y);
	pckt.read(x * y, &map[0][0]);
}

bool							Level::spawn_feeder()
{
	Player	*plr;
	Feeder	*fdr;

	for (unsigned int i = 0; i < am->_asize; ++i)
	{
		if (am->_actors[i] && (plr = dynamic_cast<Player *>(am->_actors[i])))
		{
			vec<float, 2>	loc;

			fdr = (Feeder *)am->create("Feeder", 0, true);
			fdr->hp = 1.0f + nbr / 10;
			loc[0] = (float)MT().genrand_real1(-1.0, 1.0);
			loc[1] = (float)MT().genrand_real1(-1.0, 1.0);
			fdr->bdb->loc = plr->bdb->loc + Sgl::unit(loc) * (float)MT().genrand_real1(512.0, 1024.0);
			fdr->bdb->nextloc = fdr->bdb->loc;
		}
	}
	nbr++;
	return (true);
}

bool	Level::increase_difficulty()
{
	spawnrate += 0.1f;
	update_callback("spawn_feeder", 1.0f / spawnrate);
	return (true);
}

void		Level::_generate_block()
{
	Block	blck;

	for (int j = 0; j < y; ++j)
	{
		for (int i = 0; i < x; ++i)
		{
			blck.loc[0] = i * 64.0f;
			blck.loc[1] = j * 64.0f;
			if ((blck.t = map[j][i]))
			{
				am->pe->add(&blck.bdb, 0, false);
				blck.bdb->loc = blck.loc;
				blck.bdb->size = 64.0f;
			}
			else
				blck.bdb = 0;
			_map.push_back(blck);
		}
	}
}
