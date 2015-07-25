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

#ifndef PACKET_H_
#define PACKET_H_

#include <vector>

class	Packet
{
	public:

		unsigned int		_getit;
		std::vector<char>	_data;


		Packet();
		Packet(Packet const &);
		template<class T>
		Packet(T const &);
		Packet(unsigned int const, char const *);
		~Packet();
		
		void	operator()();
		void	operator()(Packet const &);
		template<class T>
		void	operator()(T const &);
		void	operator()(unsigned int const, char const *);

		unsigned int	get_size() const;
		char const		*get_data() const;

		bool	read(unsigned int const, char *);
		bool	write(unsigned int const, char const *);

		template<class T>
		bool	read(T &);
		template<class T>
		bool	write(T const &);
};

template<class T>
Packet::Packet(T const &d) : _getit(0), _data(sizeof(d), (char *)&d)
{

}

template<class T>
void	Packet::operator()(T const &d)
{
	(*this)(sizeof(d), (char *)&d);
}

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
