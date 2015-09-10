#include "new.hpp"
#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "random/mt19937.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "callbackmanager.hpp"
#include "gameengine.hpp"
#include "physicengine.hpp"
#include "graphicengine.hpp"

#include "labyrinth.hpp"
#include "player.hpp"
#include "feeder.hpp"
#include "level.hpp"

FACTORYREG(Level);

Level::Level(Gameengine *g, Replication *r, int const i, short int const t, Actor const *o) : Actor(g, r, i, t, o), nbr(0)
{
	if (engine->master)
	{
		x = 16 * 4 + 1;
		y = x;
		map = generate_labyrinth(16, 16);
		_generate_block();
		spawnrate = 1.0f;
		engine->callback->start_callback(1, this, (bool (Actor::*)())&Level::spawn_feeder, 1.0f / spawnrate, true);
		engine->callback->start_callback(2, this, (bool (Actor::*)())&Level::increase_difficulty, 8.0f, true);
	}
}

Level::~Level()
{
	delete_matrix(map);
	for (unsigned int j = 0; j < y; ++j)
		for (unsigned int i = 0; i < x; ++i)
			if (bodies[j][i])
				engine->physic->delete_body(bodies[j][i]);
	delete_matrix(bodies);
	if (engine->graphic)
		engine->graphic->remove(ld);
}

void	Level::postinstanciation()
{
	Actor::postinstanciation();
	if (!engine->master)
		_generate_block();
	if (engine->graphic)
	{
		ld = new Leveldisplayer(engine->graphic, -1.0f, this);
		engine->graphic->add(ld);
	}
}

void	Level::get_replication(Packet &pckt) const
{
	pckt.write(ownerid);
	pckt.write(x);
	pckt.write(y);
	pckt.write(x * y, &map[0][0]);
}

void	Level::replicate(Packet &pckt, float const p)
{
	ping = p;
	pckt.read(ownerid);
	pckt.read(x);
	pckt.read(y);
	pckt.read(x * y, &map[0][0]);
}

bool		Level::spawn_feeder()
{
	Player	*plr;
	Feeder	*fdr;

	for (unsigned int i = 0; i < engine->_actsize; ++i)
	{
		if (engine->_actors[i] && (plr = dynamic_cast<Player *>(engine->_actors[i])))
		{
			vec<float, 2>	loc;

			fdr = (Feeder *)engine->create("Feeder", 0, true);
			fdr->hp = 1.0f + nbr / 10;
			loc[0] = (float)MT().genrand_real1(-1.0, 1.0);
			loc[1] = (float)MT().genrand_real1(-1.0, 1.0);
			fdr->body->position = plr->body->position + unit<float>(loc) * (float)MT().genrand_real1(512.0, 1024.0);
			break;
		}
	}
	nbr++;
	return (true);
}

bool	Level::increase_difficulty()
{
	spawnrate += 0.1f;
	engine->callback->update_callback(1, this, 1.0f / spawnrate);
	return (true);
}

void		Level::_generate_block()
{
	shape.size = 64.0f;
	bodies = new_matrix<Body *>(x, y);
	for (unsigned int j = 0; j < y; ++j)
	{
		for (unsigned int i = 0; i < x; ++i)
		{
			if (map[j][i])
			{
				engine->physic->new_body(&bodies[j][i], &shape, this);
				bodies[j][i]->dynamic = false;
				bodies[j][i]->position[0] = i * 64.0f;
				bodies[j][i]->position[1] = j * 64.0f;
				engine->physic->init_body(bodies[j][i]);
			}
			else
				bodies[j][i] = 0;
		}
	}
}
