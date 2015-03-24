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

#ifndef REPLICATION_H_
#define REPLICATION_H_

#include "endian/packet.hpp"

class	Networkengine;

class	Replication
{
	public:

		static int	get_id(Packet const &);


		Networkengine	*ne;

		short int		type;
		int 			id;
		int				numin;
		int				numout;

		float	updatetime;
		float	timeout;
		float	lastsendupd;
		float	lastrecvupd;
		bool	needupdate;
		bool	dead;
		bool	master;


		Replication(Packet &, float);
		Replication(float, float);
		virtual ~Replication();

		Packet	*get_replication();
        void	replicate(Packet &, float);

        bool	tick(float);

        void	destroy();
};

#endif
