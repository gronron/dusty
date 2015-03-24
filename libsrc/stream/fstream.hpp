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

#ifndef FILE_STREAM_H_
#define FILE_STREAM_H_

#include "stream.hpp"

class File_stream
{
	public:

		int id;

		File_stream();
		File_stream(char *name, int flag);
		File_stream(char *name, int flag, int mode);
		virtual ~File_stream();

		bool	operator()();
		bool	operator()(char *name, int flag);
		bool	operator()(char *name, int flag, int mode);

		bool	is_good() const;
};

class Ifstream : public Istream, public File_stream
{
	public:

		Ifstream();
		Ifstream(char *name);

		int	read(uint size, void *data);
};

class Ofstream : public Ostream, public File_stream
{
	public:

		Ofstream();
		Ofstream(char *name, int flag);
		Ofstream(char *name, int flag, int mode);

		int	write(uint size, void *data);
};

class Fstream : public Stream, public File_stream
{
	public:

		Fstream();
		Fstream(char *name, int flag);
		Fstream(char *name, int flag, int mode);

		int read(uint size, void *data);
		int	write(uint size, void *data);
};

#endif

