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

#ifndef PACKET_H_
#define PACKET_H_

#include <vector>

class	Packet
{
	public:

		unsigned int		_getit;
		std::vector<char>	_data;


		Packet();
		Packet(unsigned int, char const *);
		~Packet();

		unsigned int	getsize() const;
		char const		*getdata() const;

		void	clear();

		bool	read(unsigned int, char *);
		bool	write(unsigned int, char const *);

		template<class T>
		bool	read(T &);
		template<class T>
		bool	write(T const &);
};

template<class T>
bool	Packet::read(T &d)
{
	if ((_data.size() - _getit) < sizeof(T))
		return (false);
	d = *reinterpret_cast<T *>(&_data[_getit]);
	_getit += sizeof(d);
	return (true);
}

template<class T>
bool	Packet::write(T const &d)
{
	return (write(sizeof(d), (char *)&d));
}

#endif
