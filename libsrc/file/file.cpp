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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"0
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

#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file.hpp"

#if defined(_WIN32) || defined(__WIN32__)

#include <io.h>

char		*read_file(char const *filename)
{
	int		fd;
	int		size;
	int		rdsize;
	char	*buffer;

	buffer = 0;
	_sopen_s(&fd, filename, _O_RDONLY, _SH_DENYNO, _S_IREAD);
	if (fd >= 0)
	{
		size = _lseek(fd, 0, SEEK_END);
		_lseek(fd, 0, SEEK_SET);
		if (size >= 0)
		{
			buffer = new char[size + 1];
			rdsize = _read(fd, buffer, size);
			if (rdsize >= 0)
				buffer[rdsize] = '\0';
			else
			{
				delete [] buffer;
				buffer = 0;
			}
		}
		else
			perror("read_file(): ");
		_close(fd);
	}
	else
		perror("read_file(): ");
	return (buffer);
}

#else
	
#include <unistd.h>

char		*read_file(char const *filename)
{
	int		fd;
	int		size;
	int		rdsize;
	char	*buffer;

	buffer = 0;
	open(&fd, filename, O_RDONLY);
	if (fd >= 0)
	{
		size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		if (size >= 0)
		{
			buffer = new char[size + 1];
			rdsize = read(fd, buffer, size);
			if (rdsize >= 0)
				buffer[rdsize] = '\0';
			else
			{
				delete [] buffer;
				buffer = 0;
			}
		}
		else
			perror("read_file(): ");
		close(fd);
	}
	else
		perror("read_file(): ");
	return (buffer);
}

#endif
