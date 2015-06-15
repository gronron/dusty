#ifndef ACONTROLLER_H_
#define ACONTROLLER_H_

#include "controller.hpp"
#include "player.hpp"

class	AController : public Controller
{
	public:

		Player			*controlled;

		bool			firing;
		bool			loadingfire;
		vec<float, 2>	aim;
		vec<float, 2>	move;


		AController(Actormanager *, Replication *, int, short int, Actor const *);
		virtual ~AController();

		void	postinstanciation();

		void	notified_by_owned(Actor *, bool);

		void	get_replication(Packet &);
        void	replicate(Packet &, float);

		void	tick(float delta);

		void	bind();


		void	forward(int size, float *data);
		void	backward(int size, float *data);
		void	left(int size, float *data);
		void	right(int size, float *data);

		void	movex(int size, float *data);
		void	movey(int size, float *data);
		void	fire(int size, float *data);
		void	strongfire(int size, float *data);
		void	aimloc(int size, float *data);
		void	aimdirx(int size, float *data);
		void	aimdiry(int size, float *data);
};

#endif
