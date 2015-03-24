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

#ifndef NETPROTO_H_
#define NETPROTO_H_

struct					Header
{
	short unsigned int	size;
	char				type;
};

enum	Prototype
{
	CNTID,
	FPING,
	SPING,
	TPING,
	MID,
	TEXTMSG
};

class	Ping
{
	public:

		float	ping;
		float	timer;
		int		splnbr;
		int		idx;
		float	*tempping;
		bool	pinging;

		Ping(int const sn);
		~Ping();

		void	start();
		void	stop();
};

#endif
