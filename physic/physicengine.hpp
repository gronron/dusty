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

#ifndef PHYSICENGINE_H_
#define PHYSICENGINE_H_

#include "math\vec.hpp"

class	Physicengine
{
	public:
	
		struct	Boundingbox
		{
			vec<float, 3>	loc;
			vec<float, 3>	spd;
			vec<float, 3>	acc;

			vec<float, 3>	nextloc;
			vec<float, 3>	nextspd;
			vec<float, 3>	size;
			

			void	get_replication(Packet &);
			void	replicate(Packet &, float);
		};
	
		struct	Body
		{
			Boundingbox				*bb;
			unsigned int			vtcnbr;
			vec<float, 3>			*vertices;
			unsigned int			trinbr;
			vec<unsigned int, 3>	*triangles;
		};

		struct				Proxy
		{
			Boundingbox		*bb;
			vec<float, 3>	bot;
			vec<float, 3>	top;
			bool			dynamic;
		};


		unsigned int	_sbbsize;
		unsigned int	_sbbnbr;
		Boundingbox		*_staticbb;
		unsigned int	_dbbsize;
		unsigned int	_dbbnbr;
		Boundingbox		*_dynamicbb;

		unsigned int	_prxsize;
		unsigned int	_prxnbr;
		Proxy			*_sortedprx;
		unsigned int	_faxis;
		unsigned int	_saxis;
		unsigned int	_taxis;


		Physicengine();
		~Physicengine();

		void	add(Body *);
		void	remove(Body *);

		void	tick(float);

		void	_insertion_sort();
		void	_check_overlap(float);
		void	_collide(float, Boundingbox *, Boundingbox *);
		void	_collide2(float, Boundingbox *, Boundingbox *);
		bool	_reaction(Boundingbox *, Boundingbox *, float, unsigned int);
};

#endif
