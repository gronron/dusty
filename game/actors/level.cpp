#include "central/new.hpp"
#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "mt/mt19937.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"

#include "labyrinth.hpp"
#include "player.hpp"
#include "feeder.hpp"
#include "exit.hpp"
#include "level.hpp"

FACTORYREG(Level);

Level::Level(Actormanager *a, Replication *r, short int t, int i, Actor const *o) : Actor(a, r, t, i, o), nbr(0)
{
	if (!am->local && !rp)
		rp = new Replication(this, 2.0f, 20.0f);
	if (am->master)
	{
		x = 16 * 4 + 1;
		y = x;
		map = generate_labyrinth(16, 16);
		_generate_block();
		Exit	*exit = (Exit *)am->create("Exit", 0);
		exit->bd.loc = (x - 2) * 64.0f;
		spawnrate = 1.0f;
		start_callback("spawn_feeder", 1.0f / spawnrate, true, (bool (Actor::*)())&Level::spawn_feeder);
		start_callback("increase_difficulty", 8.0f, true, (bool (Actor::*)())&Level::increase_difficulty);
	}
}

Level::~Level()
{
	delete_matrix(map);
	for (unsigned int i = 0; i < _map.size(); ++i)
	{
		if (_map[i].bd)
		{
			am->pe->remove(_map[i].bd);
			delete _map[i].bd;
		}
	}
	if (am->graphic)
		ld->destroyed = true;
}

void	Level::postinstanciation()
{
	if (!am->master)
		_generate_block();
	if (am->graphic)
	{
		ld = new Leveldisplayer(-1.0f, &cleared, this);
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
	Actormanager::Actoriterator	i;
	Player	*plr;
	Feeder	*fdr;

	for (i = am->_actormap.begin(); i != am->_actormap.end(); ++i)
	{
		if ((plr = dynamic_cast<Player *>(i->second)))
		{
			vec<float, 2>	loc;

			fdr = (Feeder *)am->create("Feeder", 0);
			fdr->hp = 1.0f + nbr / 10;
			loc[0] = MT().genrand_real1(-1.0, 1.0);
			loc[1] = MT().genrand_real1(-1.0, 1.0);
			fdr->bd.loc = plr->bd.loc + Sgl::unit(loc) * (float)MT().genrand_real1(512.0, 1024.0);
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
				blck.bd = new Body(this, false, true);
				blck.bd->loc = blck.loc;
				blck.bd->size = 64.0f;
				am->pe->add(blck.bd);
			}
			else
				blck.bd = 0;
			_map.push_back(blck);
		}
	}
}
