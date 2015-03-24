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

#ifndef SELECTOR_H_
#define SELECTOR_H_

#include <list>
#include "socket.hpp"
#include "stream.hpp"

class	Selector
{
    public:

		fd_set				_rfds;
        std::list<Socket>	_strmlist;


		Selector();
		~Selector();

		template<class T>
        void	addsocket(T const &);
        template<class T>
        void	rmsocket(T const &);
        template<class T>
        bool	is_ready(T const &);

        bool	check(float timeout);
};

template<class T>
void	Selector::addsocket(T const &x)
{
	_strmlist.push_back(x._id);
}

template<class T>
void	Selector::rmsocket(T const &x)
{
	_strmlist.remove(x._id);
}

template<class T>
bool	Selector::is_ready(T const &x)
{
	return (FD_ISSET(x._id, &_rfds));
}

#endif
