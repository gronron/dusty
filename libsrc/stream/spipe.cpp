#include <stdio.h>
#include "stream.hpp"

#include "tcpstream.hpp"
#include "fstream.hpp"
#include "ioxstream.hpp"

#define BUFF_SIZE	65536

int	spipe(Istream &in, Ostream &out)
{
	int		size;
	char	buff[BUFF_SIZE];

	do
	{
		if ((size = in.read(BUFF_SIZE, buff)) < 0)
		{
			perror("Istream::read()");
			return (false);
		}
		else if (out.write(size, buff) < size)
		{
			perror("Ostream::write()");
			return (false);
		}
	}
	while (size > 0);
	return (true);
}

int	spipe(Istream &in, Ostream &out, int buff_size)
{
	int		size;
	char	*buff;

	if (!(buff = new char[buff_size]))
		return (false);
	do
	{
		if ((size = in.read(buff_size, buff)) < 0)
		{
			perror("Istream::read()");
			delete [] buff;
			return (false);
		}
		else if (out.write(size, buff) < size)
		{
			perror("Ostream::write()");
			delete [] buff;
			return (false);
		}
	}
	while (size > 0);
	delete [] buff;
	return (true);
}

//#define SERVER
#ifndef SERVER

int main()
{
	Ixstream ifs;
	Istream *in = &ifs;
	
	Otcpstream otcps("::1", "25616");
	Ostream *out = &otcps;
	
	spipe(*in, *out);
	return (0);
}

#else

int main()
{
	Tcp_server srv("25616");
	Itcpstream itcps(srv);
	Istream *in = &itcps;

	Oxstream oxs;
	Ostream *out = &oxs;

	spipe(*in, *out);
	return (0);
}

#endif
