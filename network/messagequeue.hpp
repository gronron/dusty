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

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <string>
#include "endian/packet.hpp"

class	Messagequeue
{
	public:

		enum		Target { GLBROADCAST, LCBROADCAST, GLTEAM, LCTEAM, PLAYERS };
		enum		Type { CONNECTION, DISCONNECTION, UPDATE, DESTROY, CONTROL, TEXTMSG };

		struct		Message
		{
			char	target;
			char	type;
			int		cltid;
			int		actid;
			float	ping;
			Packet	*pckt;
		};


		unsigned int	_size;
		unsigned int	_frontin;
		unsigned int	_frontout;
		unsigned int	_backin;
		unsigned int	_backout;
		Message			*_msgin;
		Message			*_msgout;


		Messagequeue();
		~Messagequeue();

		void	push_in_pckt(int cltid, float ping, int size, char *data);
		void	push_in_cnt(int cltid);
		void	push_in_discnt(int cltid);
		void	push_in_cntrl(int actid);
		void	push_in_textmsg(int size, char *data);
		void	push_in_textmsg(int cltid, int size, char *data);

		void	push_out_pckt(Packet *pckt);
		void	push_out_pckt(int actorid);
		void	push_out_cnt(int cltid, int actid);
		void	push_out_discnt(int cltid);
		void	push_out_textmsg(std::string const &msg);
		void	push_out_textmsg(int cltid, std::string const &msg);

		Message	*get_in();
		void	pop_in();
		Message	*get_out();
		void	pop_out();
};

#endif
