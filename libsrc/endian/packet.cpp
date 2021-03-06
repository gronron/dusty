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

#include <string.h>
#include "packet.hpp"

Packet::Packet() : _getit(0)
{

}

Packet::Packet(Packet const &pckt) : _getit(pckt._getit), _data(pckt._data)
{

}


Packet::Packet(unsigned int const size, char const *d) : _getit(0), _data(d, d + size)
{

}

Packet::~Packet()
{

}

void	Packet::operator()()
{
	_getit = 0;
	_data.clear();
}

void	Packet::operator()(Packet const &pckt)
{
	_getit = pckt._getit;
	_data = pckt._data;
}

void	Packet::operator()(unsigned int const size, char const *d)
{
	_getit = 0;
	_data.resize(size);
	memcpy(&_data[0], d, size);
}

unsigned int	Packet::get_size() const
{
	return ((unsigned int)_data.size());
}

char const	*Packet::get_data() const
{
	return (_data.empty() ? 0 : &_data[0]);
}

bool	Packet::read(unsigned int const size, char *d)
{
	if ((_data.size() - _getit) < size)
		return (false);
	memcpy(d, &_data[_getit], size);
	_getit += size;
	return (true);
}

bool				Packet::write(unsigned int const size, char const *d)
{
	size_t const	a = _data.size();
	_data.resize(a + size);
	memcpy(&_data[a], d, size);
	return (true);
}
