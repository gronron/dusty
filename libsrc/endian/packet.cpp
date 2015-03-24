///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2014 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of libsrc.
//
//    libsrc is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsrc is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsrc.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "packet.hpp"

Packet::Packet() : _getit(0)
{

}

Packet::Packet(unsigned int size, char const *d) : _getit(0), _data(d, d + size)
{

}

Packet::~Packet()
{

}

unsigned int			Packet::getsize() const
{
	return (_data.size());
}

char const	*Packet::getdata() const
{
	return (_data.empty() ? 0 : &_data[0]);
}

void	Packet::clear()
{
	_getit = 0;
	_data.clear();
}

bool	Packet::read(unsigned int size, char *d)
{
	if ((_data.size() - _getit) < size)
		return (false);
	memcpy(d, &_data[_getit], size);
	_getit += size;
	return (true);
}

bool	Packet::write(unsigned int size, char const *d)
{
	int	a = _data.size();
	_data.resize(a + size);
	memcpy(&_data[a], d, size);
	return (true);
}
