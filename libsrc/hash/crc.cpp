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

#include "crc.hpp"

unsigned int		crc32(char const * str)
{
	unsigned int	crc = 0xFFFFFFFF;

	while (*str)
		crc = crc32_table[(crc ^ *str++) & 0x000000FF] ^ (crc >> 8);
	return (crc ^ 0xFFFFFFFF);
}

unsigned int	crc32(unsigned int crc, char const * str)
{
	while (*str)
		crc = crc32_table[(crc ^ *str++) & 0x000000FF] ^ (crc >> 8);
	return (crc ^ 0xFFFFFFFF);
}

unsigned long long int		crc64(char const * str)
{
	unsigned long long int	crc = 0xFFFFFFFFFFFFFFFF;

	while (*str)
		crc = crc64_table[(crc ^ *str++) & 0x00000000000000FF] ^ (crc >> 8);
	return (crc ^ 0xFFFFFFFFFFFFFFFF);
}

unsigned long long int	crc64(unsigned long long int crc, char const * str)
{
	while (*str)
		crc = crc64_table[(crc ^ *str++) & 0x00000000000000FF] ^ (crc >> 8);
	return (crc ^ 0xFFFFFFFFFFFFFFFF);
}
