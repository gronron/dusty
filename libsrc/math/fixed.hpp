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

#pragma once

template<unsigned int FractionalBits, class Type, class SuperType>
struct fixed
{
	static constexpr const Type		factor = (Type)1 << FractionalBits;
	static constexpr const float	inv_factor = 1.0f / factor;
	static constexpr const double	inv_factor_d = 1.0 / factor;


	Type	value;


	fixed() : value(0) {}
	explicit fixed(Type const x) : value(x) {}
	explicit fixed(float const x) : value(x * factor) {}
	explicit fixed(double const x) : value(x * factor) {}

	explicit operator	float() const { return (value * inv_factor); }
	explicit operator	double() const { return (value * inv_factor_d); }

	fixed	&operator=(fixed const x) { value = x.value; return (*this); }
	fixed	&operator+=(fixed const x) { value += x.value; return (*this); }
	fixed	&operator-=(fixed const x) { value -= x.value; return (*this); }
	fixed	&operator*=(fixed const x) { value = ((SuperType)value * x.value) >> FractionalBits; return (*this); }
	fixed	&operator/=(fixed const x) { value = ((SuperType)value << FractionalBits) / x.value; return (*this); }
	fixed	&operator%=(fixed const x) { value = x.value; return (*this); }

	fixed	operator+(fixed const x) const { return (fixed(value + x.value)); }
	fixed	operator-(fixed const x) const { return (fixed(value - x.value)); }
	fixed	operator*(fixed const x) const { return (fixed(((SuperType)value * x.value) >> FractionalBits)); }
	fixed	operator/(fixed const x) const { return (fixed(((SuperType)value << FractionalBits) / x.value)); }

	bool	operator==(fixed const x) const { return (value == x.value); }
	bool	operator!=(fixed const x) const { return (value != x.value); }
	bool	operator<(fixed const x) const { return (value < x.value); }
	bool	operator>(fixed const x) const { return (value > x.value); }
	bool	operator<=(fixed const x) const { return (value <= x.value); }
	bool	operator>=(fixed const x) const { return (value >= x.value); }
};
