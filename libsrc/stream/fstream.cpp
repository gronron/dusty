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

