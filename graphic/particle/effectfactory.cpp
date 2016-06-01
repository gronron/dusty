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

#include <iostream>
#include <cstdlib>
#include "new.hpp"
#include "effectfactory.hpp"

Effectfactory		&Effectfactory::get_instance()
{
	static Effectfactory	instance;

	return (instance);
}

Effectfactory::Effectfactory() : _prcount(0), _prsize(64), _pairs(0)
{
	_pairs = new Pair[_prsize];
}

Effectfactory::~Effectfactory()
{
	delete [] _pairs;
}

void	Effectfactory::register_class(unsigned int const hash, CF cf)
{
	if (_prcount >= _prsize)
	{
		_prsize <<= 1;
		_pairs = resize(_pairs, _prcount, _prsize);
	}

	unsigned int	i;
	for (i = _prcount++; i && _pairs[i - 1].hash > hash; --i)
	{
		if (_pairs[i - 1].hash == hash)
		{
			std::cerr << "error! Effectfactory::register_class(): hash collision" << std::endl;
			exit(EXIT_FAILURE);
		}
		_pairs[i] = _pairs[i - 1];
	}
	_pairs[i].hash = hash;
	_pairs[i].cf = cf;
}

Particleeffect		*Effectfactory::create(unsigned int const hash, Particlesystem *p, float const t, Df_node const *d) const
{
	int				imin = 0;
	int				imax = _prcount - 1;

	while (imin <= imax)
	{
		int const	imid = (imin + imax) >> 1;

		if (hash < _pairs[imid].hash)
			imax = imid - 1;
		else if (hash > _pairs[imid].hash)
			imin = imid + 1;
		else
			return (_pairs[imid].cf(p, t, d));
	}
	std::cerr << "Error! Effectfactory::create() fails to find hash: " << hash << std::endl;
	exit(EXIT_FAILURE);
	return (0);
}
