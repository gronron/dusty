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

#include "endian.hpp"

union		u_int16
{
	short	a;
	char	c[2];
};

union		u_int32
{
	int		a;
	char	c[4];
};

union			u_int64
{
	long long	a;
	char		c[8];
};

bool		isbigendian()
{
	u_int16	a;

	a.a = 1;
	return (!a.c[0]);
}

short		switch_endianess(short x)
{
	u_int16	a;
	u_int16	b;

	b.a = x;
	a.c[0] = b.c[1];
	a.c[1] = b.c[0];
	return (a.a);
}

int			switch_endianess(int x)
{
	u_int32	a;
	u_int32	b;

	b.a = x;
	a.c[0] = b.c[3];
	a.c[1] = b.c[2];
	a.c[2] = b.c[1];
	a.c[3] = b.c[0];
	return (a.a);
}

long long	switch_endianess(long long x)
{
	u_int64	a;
	u_int64	b;

	b.a = x;
	a.c[0] = b.c[7];
	a.c[1] = b.c[6];
	a.c[2] = b.c[5];
	a.c[3] = b.c[4];
	a.c[4] = b.c[3];
	a.c[5] = b.c[2];
	a.c[6] = b.c[1];
	a.c[7] = b.c[0];
	return (a.a);
}
