///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2012 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of Chasis.
//
//    Chasis is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Chasis is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Chasis.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef STREAM_H_
#define STREAM_H_

class Istream
{
	public:

		virtual ~Istream() {};

		virtual int		read(unsigned int size, void *data) = 0;
};

class Ostream
{
	public:

		virtual ~Ostream() {};

		virtual int		write(unsigned int size, void const *data) = 0;
};

class Stream : public Istream, public Ostream
{
	public:

		virtual ~Stream() {};
};

#endif
