#ifndef IOX_STREAM
#define IOX_STREAM

#include "stream.hpp"

class Ixstream : public Istream
{
	int	read(uint size, void *data);
};

class Oxstream : public Ostream
{
	int write(uint size, void *data);
};

class Exstream : public Ostream
{
	int write(uint size, void *data);
};

#endif
