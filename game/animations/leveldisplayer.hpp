#ifndef LEVELDISPLAYER_H_
#define LEVELDISPLAYER_H_

#include "animation.hpp"
#include "renderer.hpp"

class	Level;

class	Leveldisplayer : public Animation
{
	public:

		Level					*lvl;
		Renderer::Texture	*tex;

		Leveldisplayer(float, bool *, Level *);
		virtual ~Leveldisplayer();

		void	post_instanciation(Renderer *);

		bool	tick(float);
		void	draw();
};

#endif
