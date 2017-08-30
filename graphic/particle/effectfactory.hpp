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

#ifndef EFFECTFACTORY_H_
#define EFFECTFACTORY_H_

#include "hash/crc.hpp"

#define EFFECTFACTORYREG(name) Effectfactoryregister<name> const	registered_effect_##name(CRC32(#name))

class	Df_node;
class	Particleeffect;
class	Particlesystem;

class	Effectfactory
{
	public:

		typedef Particleeffect *(*CF)(Particlesystem *, float const, Df_node const *);

		struct	Pair
		{
			unsigned int	hash;
			CF				cf;
		};


		static Effectfactory	&get_instance();


		unsigned int	_prcount;
		unsigned int	_prsize;
		Pair			*_pairs;


		Effectfactory();
		~Effectfactory();

		void			register_class(unsigned int const, CF);
		Particleeffect	*create(unsigned int const, Particlesystem *, float const, Df_node const *) const;
};

template<class T>
Particleeffect	*create_effect(Particlesystem *p, float const t, Df_node const *d)
{
	return (new T(p, t, d));
}

template<class T>
struct	Effectfactoryregister
{
	Effectfactoryregister(unsigned int const hash)
	{
		Effectfactory::get_instance().register_class(hash, &create_effect<T>);
	}
};

#endif
