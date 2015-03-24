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
