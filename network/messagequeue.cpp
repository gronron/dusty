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

#include "messagequeue.hpp"

Messagequeue::Messagequeue() : _size(4096), _frontin(0), _frontout(0), _backin(0), _backout(0)
{
	_msgin = new Message[_size];
	_msgout = new Message[_size];
}

Messagequeue::~Messagequeue()
{
	while (_frontin != _backin)
	{
		delete _msgin[_backin].pckt;
		_backin = (_backin + 1) % _size;
	}
	while (_frontout != _backout)
	{
		delete _msgout[_backout].pckt;
		_backout = (_backout + 1) % _size;
	}
	delete [] _msgin;
	delete [] _msgout;
}

///////////////////////////////////////

void	Messagequeue::push_in_pckt(int cltid, float ping, int size, char *data)
{
	if ((_frontin + 1) % _size != _backin)
	{
		_msgin[_frontin].type = size == 4 ? DESTROY : UPDATE;
		_msgin[_frontin].cltid = cltid;
		_msgin[_frontin].actid = 0;
		_msgin[_frontin].ping = ping;
		_msgin[_frontin].pckt = new Packet(size, data);
		_frontin = (_frontin + 1) % _size;
	}
}

void	Messagequeue::push_in_cnt(int cltid)
{
	if ((_frontin + 1) % _size != _backin)
	{
		_msgin[_frontin].type = CONNECTION;
		_msgin[_frontin].cltid = cltid;
		_msgin[_frontin].actid = 0;
		_msgin[_frontin].ping = 0.0f;
		_msgin[_frontin].pckt = 0;
		_frontin = (_frontin + 1) % _size;
	}
}

void	Messagequeue::push_in_discnt(int cltid)
{
	if ((_frontin + 1) % _size != _backin)
	{
		_msgin[_frontin].type = DISCONNECTION;
		_msgin[_frontin].cltid = cltid;
		_msgin[_frontin].actid = 0;
		_msgin[_frontin].ping = 0.0f;
		_msgin[_frontin].pckt = 0;
		_frontin = (_frontin + 1) % _size;
	}
}

void	Messagequeue::push_in_cntrl(int actid)
{
	if ((_frontin + 1) % _size != _backin)
	{
		_msgin[_frontin].type = CONTROL;
		_msgin[_frontin].cltid = 0;
		_msgin[_frontin].actid = actid;
		_msgin[_frontin].ping = 0.0f;
		_msgin[_frontin].pckt = 0;
		_frontin = (_frontin + 1) % _size;
	}
}

void	Messagequeue::push_in_textmsg(int size, char *data)
{
	if ((_frontin + 1) % _size != _backin)
	{
		_msgin[_frontin].type = TEXTMSG;
		_msgin[_frontin].cltid = 0;
		_msgin[_frontin].actid = 0;
		_msgin[_frontin].ping = 0.0f;
		_msgin[_frontin].pckt = new Packet(size, data);
		_frontin = (_frontin + 1) % _size;
	}
}

void	Messagequeue::push_in_textmsg(int cltid, int size, char *data)
{
	if ((_frontin + 1) % _size != _backin)
	{
		_msgin[_frontin].type = TEXTMSG;
		_msgin[_frontin].cltid = cltid;
		_msgin[_frontin].actid = 0;
		_msgin[_frontin].ping = 0.0f;
		_msgin[_frontin].pckt = new Packet(size, data);
		_frontin = (_frontin + 1) % _size;
	}
}

///////////////////////////////////////

void		Messagequeue::push_out_pckt(Packet *pckt)
{
	if ((_frontout + 1) % _size != _backout)
	{
		_msgout[_frontout].type = UPDATE;
		_msgout[_frontout].cltid = 0;
		_msgout[_frontout].actid = 0;
		_msgout[_frontout].ping = 0.0f;
		_msgout[_frontout].pckt = pckt;
		_frontout = (_frontout + 1) % _size;
	}
}

void		Messagequeue::push_out_pckt(int actorid)
{
	if ((_frontout + 1) % _size != _backout)
	{
		_msgout[_frontout].type = DESTROY;
		_msgout[_frontout].cltid = 0;
		_msgout[_frontout].actid = 0;
		_msgout[_frontout].ping = 0.0f;
		_msgout[_frontout].pckt = new Packet;
		_msgout[_frontout].pckt->write(actorid);
		_frontout = (_frontout + 1) % _size;
	}
}

void	Messagequeue::push_out_cnt(int cltid, int actid)
{
	if ((_frontout + 1) % _size != _backout)
	{
		_msgout[_frontout].type = CONNECTION;
		_msgout[_frontout].cltid = cltid;
		_msgout[_frontout].actid = actid;
		_msgout[_frontout].ping = 0.0f;
		_msgout[_frontout].pckt = 0;
		_frontout = (_frontout + 1) % _size;
	}
}

void	Messagequeue::push_out_discnt(int cltid)
{
	if ((_frontout + 1) % _size != _backout)
	{
		_msgout[_frontout].type = DISCONNECTION;
		_msgout[_frontout].cltid = cltid;
		_msgout[_frontout].actid = 0;
		_msgout[_frontout].ping = 0.0f;
		_msgout[_frontout].pckt = 0;
		_frontout = (_frontout + 1) % _size;
	}
}

void	Messagequeue::push_out_textmsg(std::string const &str)
{
	if ((_frontout + 1) % _size != _backout)
	{
		_msgout[_frontout].type = TEXTMSG;
		_msgout[_frontout].cltid = 0;
		_msgout[_frontout].actid = 0;
		_msgout[_frontout].ping = 0.0f;
		_msgout[_frontout].pckt = new Packet(str.length() + 1, str.c_str());
		_frontout = (_frontout + 1) % _size;
	}
}

void	Messagequeue::push_out_textmsg(int cltid, std::string const &str)
{
	if ((_frontout + 1) % _size != _backout)
	{
		_msgout[_frontout].type = TEXTMSG;
		_msgout[_frontout].cltid = cltid;
		_msgout[_frontout].actid = 0;
		_msgout[_frontout].ping = 0.0f;
		_msgout[_frontout].pckt = new Packet(str.length() + 1, str.c_str());
		_frontout = (_frontout + 1) % _size;
	}
}

///////////////////////////////////////

Messagequeue::Message	*Messagequeue::get_in()
{
	if (_frontin != _backin)
		return (_msgin + _backin);
	else
		return (0);
}

void	Messagequeue::pop_in()
{
	if (_frontin != _backin)
		_backin = (_backin + 1) % _size;
}

Messagequeue::Message	*Messagequeue::get_out()
{
	if (_frontout != _backout)
		return (_msgout + _backout);
	else
		return (0);
}

void	Messagequeue::pop_out()
{
	if (_frontout != _backout)
		_backout = (_backout + 1) % _size;
}
