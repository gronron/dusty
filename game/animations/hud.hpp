#ifndef HUD_H_
#define HUD_H_

#include "animation.hpp"

class	Player;

class	Hud : public Animation
{
	public:

		Player	*plr;

		Hud(float, bool *, Player *);
		virtual ~Hud();

		bool	tick(float);
		void	draw();
};

#endif
