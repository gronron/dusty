#ifndef LEVELDISPLAYER_H_
#define LEVELDISPLAYER_H_

#include "animation.hpp"
#include "graphicengine.hpp"

class	Level;

class	Leveldisplayer : public Animation
{
	public:

		Level					*lvl;
		Graphicengine::Texture	*tex;

		Leveldisplayer(Graphicengine *, float, Level *);
		virtual ~Leveldisplayer();

		bool	tick(float);
		void	draw();
};

#endif
