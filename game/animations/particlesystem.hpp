/******************************************************************************
Copyright (c) 2015, Geoffrey TOURON
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dusty nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include <string>
#include "boundingbox.hpp"
#include "animation.hpp"
#include "graphicengine.hpp"

class	Particlesystem : public Animation
{
	public:

		struct	Particle
		{
			vec<float, 2>	loc;
			vec<float, 2>	spd;

			vec<float, 4>	color;
			float			fade;
		};

		Graphicengine::Texture	*tex;
		Boundingbox				**boundingbox;
		
		unsigned int		_size;
		Particle			*_particles;

		bool				attractor;
		float				spawnrate;
		float				spawntime;
		float				time;

		vec<float, 3>	cmax;
		vec<float, 3>	cmin;
		vec<float, 2>	fade;
		vec<float, 2>	speed;

		Particlesystem(Graphicengine *, float, std::string const &, Boundingbox **);
		Particlesystem(Graphicengine *, float, std::string const &, vec<float, 2> const &);
		virtual ~Particlesystem();

		bool	tick(float);

		void	draw();

		void	stop();
		void	reverse();
};

#endif
