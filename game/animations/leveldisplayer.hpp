#ifndef LEVELDISPLAYER_H_
#define LEVELDISPLAYER_H_

#include "animation.hpp"

class	Level;
class	Gameengine;

class	Leveldisplayer : public Animation
{
	public:

		Level					*lvl;
		Graphicengine::Texture	*tex;

		Leveldisplayer(Gameengine *, float, Level *);
		virtual ~Leveldisplayer();

		bool	tick(float);
		void	draw();
};

#endif
