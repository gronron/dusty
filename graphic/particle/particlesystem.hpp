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
#include "body.hpp"
#include "animation.hpp"
#include "graphicengine.hpp"
#include "particleeffect.hpp"

class	Df_node;

class	Particlesystem : public Animation
{
	public:

		struct	Particle
		{
			vec<float, 4>	position;
			vec<float, 4>	velocity;

			float			size;
			float			fade_rate;
			bool			active;
		};
		
		struct	Effect
		{
			Particleeffect	*pe;
			DFNode	const	*next;
		};


		bool			running;
		vec<float, 4>	position;
		Body 			**body;
		unsigned int	material;
		float			scale;

		unsigned int	_prtclssize;
		Particle		*_particles;

		Effect			_effects[8];


		Particlesystem(Graphicengine *, std::string const &, Body **);
		Particlesystem(Graphicengine *, std::string const &, vec<float, 4> const &);
		Particlesystem(Graphicengine *, std::string const &, vec<float, 4> const &, unsigned int, Particle *);
		virtual ~Particlesystem();

		void	stop();
		void	attach(Body **);
		void	detach();

		bool	tick(float const);

		void	add_effects(DFNode const *);
		void	remove_effect();
};

#endif
