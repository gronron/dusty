#ifndef ACONTROLLER_H_
#define ACONTROLLER_H_

#include "controller.hpp"
#include "player.hpp"

class	AController : public Controller
{
	public:
	
		enum	Type { SPECTATOR, WORLDCREATOR, FIGHTER, BUILDER };

		Player			*controlled;

		char			controllertype;

		bool			firing;
		bool			loadingfire;
		vec<float, 4>	aim;
		vec<float, 4>	move;
		int				material;


		AController(Gameengine *, Replication *, int const, short int const, Entity const *);
		virtual ~AController();

		void	postinstanciation();

		void	notified_by_owned(Entity *, bool const);

		void	get_replication(Packet &) const;
        void	replicate(Packet &, float const);

		void	tick(float const delta);
		
		void	bind();

		void	change_type(char const);

		void	switch_to_spectator(int const, float const *);
		void	switch_to_worldcreator(int const, float const *);
		void	switch_to_fighter(int const, float const *);
		
		void	forward(int const, float const *);
		void	backward(int const, float const *);
		void	left(int const, float const *);
		void	right(int const, float const *);
		void	aimloc(int const, float const *);
		
		void	load(int const, float const *);
		void	save(int const, float const *);
		void	create_block(int const, float const *);
		void	destroy_block(int const, float const *);
		void	change_material(int const, float const *);

		void	fire(int const, float const *);
		void	strongfire(int const, float const *);
};

#endif
