#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "stream.hpp"
#include "fstream.hpp"

File_stream::File_stream()
{
	id = -1;
}

File_stream::File_stream(char *name, int flag)
{
	id = open(name, flag);
}

File_stream::File_stream(char *name, int flag, int mode)
{
	id = open(name, flag, mode);
}

File_stream::~File_stream()
{
	if (id >= 0)
		close(id);
}

File_stream::operator bool()
{
	return (id >= 0);
}

bool	File_stream::operator()()
{
	if (id >= 0)
	{
		close(id);
		id = -1;
	}
	return (true);
}

bool	File_stream::operator()(char *name, int flag)
{
	if (id >= 0)
	{
		close(id);
		id = -1;
	}
	if ((id = open(name, flag)) < 0)
		perror("open()");
	return (id >= 0);
}

bool	File_stream::operator()(char *name, int flag, int mode)
{
	if (id >= 0)
	{
		close(id);
		id = -1;
	}
	if ((id = open(name, flag, mode)) < 0)
		perror("open()");
	return (id >= 0);
}

Ifstream::Ifstream() : File_stream()
{

}

Ifstream::Ifstream(char *name) : File_stream(name, O_RDONLY)
{

}

Ifstream::~Ifstream()
{

}

int	Ifstream::read(uint size, void *data)
{
	return (id < 0 ? id : ::read(id, data, size));
}

///////////////////////////////////////

int	Ofstream::write(uint size, void *data)
{
	return (id < 0 ? id : ::write(id, data, size));
}

///////////////////////////////////////

int	Fstream::read(uint size, void *data)
{
	return (id < 0 ? id : ::read(id, data, size));
}

int	Fstream::write(uint size, void *data)
{
	return (id < 0 ? id : ::write(id, data, size));
}

