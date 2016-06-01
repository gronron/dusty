/******************************************************************************
Copyright (c) 2015-2016, Geoffrey TOURON
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
#include <stdlib.h>
#include "new.hpp"
#include "replication.hpp"
#include "factory.hpp"

Factory				&Factory::get_instance()
{
	static Factory	instance;

	return (instance);
}

Factory::Factory() : _classcount(0), _classsize(64), _classes(0)
{
	_classes = new Class[_classsize];
}

Factory::~Factory()
{
	delete [] _classes;
}

void		Factory::register_class(unsigned int const hash, CF cf)
{
	if (_classcount >= _classsize)
	{
		_classsize <<= 1;
		_classes = resize(_classes, _classcount, _classsize);
	}

	unsigned int	i;
	for (i = _classcount++; i && _classes[i - 1].hash > hash; --i)
	{
		if (_classes[i - 1].hash == hash)
		{
			std::cerr << "error! Factory::register_class(): hash collision" << std::endl;
			exit(EXIT_FAILURE);
		}
		_classes[i] = _classes[i - 1];
	}
	_classes[i].hash = hash;
	_classes[i].cf = cf;
}

Entity	*Factory::create(Gameengine *g, Replication *r) const
{
	if (r->type >= 0 && r->type < (int)_classcount)
		return (_classes[r->type].cf(g, r, r->id, r->type, 0));
	else
	{
		std::cerr << "Error! Factory::create() the following id doesn't exist: " << r->type << std::endl;
		exit(EXIT_FAILURE);
		return (0);
	}
}

Entity	*Factory::create(Gameengine *g, Replication *r, int const id, unsigned int const hash, Entity const *owner) const
{
	int	imin = 0;
	int	imax = _classcount - 1;

	while (imin <= imax)
	{
		int const	imid = (imin + imax) >> 1;

		if (hash < _classes[imid].hash)
			imax = imid - 1;
		else if (hash > _classes[imid].hash)
			imin = imid + 1;
		else
			return (_classes[imid].cf(g, r, id, imid, owner));
	}
	std::cerr << "Error! Factory::create() the following hash doesn't exist : " << hash << std::endl;
	exit(EXIT_FAILURE);
	return (0);
}
