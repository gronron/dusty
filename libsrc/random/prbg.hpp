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

#ifnded PRBG_H_
#define PRBG_H_

// Pseudo Random Binary Generator
// randomise a boolean with the probability to be true
// there is 100% chance to get true after ( (2 - [probability]) / [probability] ) times pinking false
// and less than [probability] to get true at first pick

class	Prbg
{
	public:
	
		float	chance;
		float	counter;
		bool	inverter;
		
		
		Prbg(float probability);
		~Prbg();

		void	operator()(float probability);
		
		bool	pick();
};

#endif
