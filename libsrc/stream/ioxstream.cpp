#include <unistd.h>
#include "stream.hpp"
#include "ioxstream.hpp"

int	Ixstream::read(uint size, void *data)
{
	return (::read(0, data, size));
}

int	Oxstream::write(uint size, void *data)
{
	return (::write(1, data, size));
}

int	Exstream::write(uint size, void *data)
{
	return (::write(2, data, size));
}
