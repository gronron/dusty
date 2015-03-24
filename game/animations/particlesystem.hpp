#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include <list>
#include <string>
#include "body.hpp"
#include "animation.hpp"
#include "graphicengine.hpp"

class	Particlesystem : public Animation
{
	public:

		class	Particle
		{
			public:

				vec<float, 2>	loc;
				vec<float, 2>	spd;

				vec<float, 4>	color;
				float			fade;
		};

		Graphicengine::Texture	*tex;

		bool				attractor;
		float				spawnrate;
		float				spawntime;
		float				time;
		std::list<Particle>	_particlelist;

		vec<float, 3>	cmax;
		vec<float, 3>	cmin;
		vec<float, 2>	fade;
		vec<float, 2>	speed;

		Particlesystem(float, bool *, std::string const &, Body *);
		Particlesystem(float, std::string const &, vec<float, 2> const &);
		virtual ~Particlesystem();

		void	post_instanciation(Graphicengine *);

		bool	tick(float);

		void	draw();

		void	stop();
		void	reverse();
};

#endif
