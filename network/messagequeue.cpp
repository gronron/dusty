///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2012 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of Chasis.
//
//    Chasis is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Chasis is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Chasis.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

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
