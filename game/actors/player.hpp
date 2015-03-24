#ifndef PLAYER_H_
#define PLAYER_H_

#include "actor.hpp"
#include "body.hpp"
#include "particlesystem.hpp"
#include "hud.hpp"

class	Player : public Actor
{
	public:

		Body	bd;

		int				tolerance;
		int				dose;
		bool			high;
		float			dmg;
		float			firerate;
		float			score;
		float			loadingtime;

		bool			firing;
		bool			loadingfire;
		vec<float, 2>	dir;

		Particlesystem	*ps;
		Hud				*hud;


		Player(Actormanager *, Replication *, short int, int, Actor const *);
		virtual ~Player();

		void	postinstanciation();
		void	destroy();

		void	notified_by_owner(Actor *, bool);

		void	get_replication(Packet &);
		void	replicate(Packet &, float);

		void	tick(float delta);

		bool	collide(Actor const &);

		bool	fire();
		bool	down();
};

#endif
