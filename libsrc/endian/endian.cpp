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

#include "endian.hpp"

union		u_int16
{
	short	a;
	char	c[2];
};

union		u_int32
{
	int		a;
	char	c[4];
};

union			u_int64
{
	long long	a;
	char		c[8];
};

bool		isbigendian()
{
	u_int16	a;

	a.a = 1;
	return (!a.c[0]);
}

short		switch_endianess(short x)
{
	u_int16	a;
	u_int16	b;

	b.a = x;
	a.c[0] = b.c[1];
	a.c[1] = b.c[0];
	return (a.a);
}

int			switch_endianess(int x)
{
	u_int32	a;
	u_int32	b;

	b.a = x;
	a.c[0] = b.c[3];
	a.c[1] = b.c[2];
	a.c[2] = b.c[1];
	a.c[3] = b.c[0];
	return (a.a);
}

long long	switch_endianess(long long x)
{
	u_int64	a;
	u_int64	b;

	b.a = x;
	a.c[0] = b.c[7];
	a.c[1] = b.c[6];
	a.c[2] = b.c[5];
	a.c[3] = b.c[4];
	a.c[4] = b.c[3];
	a.c[5] = b.c[2];
	a.c[6] = b.c[1];
	a.c[7] = b.c[0];
	return (a.a);
}
