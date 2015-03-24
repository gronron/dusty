#ifndef FILE_STREAM
#define FILE_STREAM

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

