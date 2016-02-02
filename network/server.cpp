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

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "messagequeue.hpp"
#include "server.hpp"

float const	Server::pingrate = 0.5f;
float const	Server::timeout = 5.0f;

Server::Server(Messagequeue *m, char const *port) : mq(m), _tcpsrv(port, false), _udpsrv(port, false), _maxclts(64)
{
	_clients = new Client[_maxclts];
	if (!_tcpsrv.is_good() || !_udpsrv.is_good())
		exit(EXIT_FAILURE);
	_slctr.add_socket(_tcpsrv);
	_slctr.add_socket(_udpsrv);
}

Server::~Server()
{
	delete [] _clients;
	//delete ping ??
}

void	Server::tick(float const delta)
{
	_pingclient(delta);
	while (_slctr.check(0.0f))
	{
		if (_slctr.is_ready(_tcpsrv))
			_addclient();
		if (_slctr.is_ready(_udpsrv))
			_receivepacket();
		for (unsigned int i = 0; i < _maxclts; ++i)
			if (_clients[i].tcp.is_good() && _slctr.is_ready(_clients[i].tcp))
				_comtcpclient(i);
	}
	_sendpacket();
}

void				Server::_addclient()
{
	unsigned int	id;
	Header			hdr;

	for (id = 0; id < _maxclts; ++id)
	{
		if (!_clients[id].tcp.is_good())
			break;
	}
	if (id >= _maxclts)
		; //don't accept client
	
	_clients[id].tcp(_tcpsrv, _clients[id].ip, _clients[id].port);
	if (!_clients[id].tcp.is_good())
		return;
	hdr.size = sizeof(id);
	hdr.type = CNTID;
	if (_clients[id].tcp.write(sizeof(Header), &hdr) < 0 || _clients[id].tcp.write(hdr.size, &id) < 0)
	{
		std::cerr << "Warning: Server::_addclient() fails to send connection id to: " << _clients[id].ip << ":" << _clients[id].port << std::endl;
		_clients[id].tcp();
		return;
	}
	_clients[id].udpid = -1;
	_clients[id].ping = new Ping(8);
	_slctr.add_socket(_clients[id].tcp);
	mq->push_in_cnt(id);
}

void	Server::_delclient(int const id)
{
	std::cout << "Client disconected" << std::endl;
	mq->push_in_discnt(id);
	_udpsrv.rm_client(_clients[id].udpid);
	_slctr.rm_socket(_clients[id].tcp);
	_clients[id].tcp();
	delete _clients[id].ping;
}

void		Server::_pingclient(float const delta)
{
	Header	hdr;

	for (unsigned int i = 0; i < _maxclts; ++i)
	{
		if  (_clients[i].ping->pinging)
		{
			if ((_clients[i].ping->tempping[_clients[i].ping->idx] += delta) > timeout)
				_delclient(i);
		}
		else
		{
			if ((_clients[i].ping->timer += delta) >= pingrate)
			{
				_clients[i].ping->start();
				hdr.type = FPING;
				_clients[i].tcp.write(sizeof(hdr), &hdr);
			}
		}
	}
}

bool		Server::_comtcpclient(int const id)
{
	Header	hdr;
	int		size;
	char	data[Udpstream::MAXUDPSIZE];

	if ((size = _clients[id].tcp.read(sizeof(hdr), &hdr)) > 0)
	{
		if (hdr.type == SPING)
		{
			hdr.type = TPING;
			_clients[id].tcp.write(sizeof(hdr), &hdr);
			_clients[id].ping->stop();
			return (true);
		}
		else if (hdr.type == TEXTMSG)
		{
			if ((size = _clients[id].tcp.read(hdr.size, data)) > 0)
			{
				mq->push_in_textmsg(id, size, data);
				return (true);
			}
		}
		else
			std::cerr << "Warning: Server::_comtcpclient() receives bad data from: " << _clients[id].ip << ":" << _clients[id].port << std::endl;
	}
	_delclient(id);
	return (false);
}

void		Server::_receivepacket()
{
	char	a[Udpstream::MAXUDPSIZE];
	int		size;
	int		id;
	int		udpid;

	if ((size = _udpsrv.read(udpid, Udpstream::MAXUDPSIZE, a)) > 0)
	{
		if (udpid >= 0)
		{
			for (unsigned int i = 0; i < _maxclts; ++i)
			{
				if (_clients[i].udpid == udpid && _clients[i].tcp.is_good())
				{
					mq->push_in_pckt(i, _clients[i].ping->ping, size, a);
					break;
				}
			}

		}
		else if (size == sizeof(short int))
		{
			memcpy(&id, a, sizeof(short int));
			if (id >= 0 && id < (int)_maxclts && _clients[id].tcp.is_good() && _clients[id].udpid < 0)
				_clients[id].udpid = _udpsrv.add_client();
		}
	}
	else if (size < 0)
		std::cerr << "Warning: Server::_receivepacket() fails to read on udp" << std::endl;
}

void						Server::_sendpacket()
{
	Messagequeue::Message	*msg;
	Header					hdr;

	while ((msg = mq->get_out()))
	{
		if (msg->type == Messagequeue::UPDATE || msg->type == Messagequeue::DESTROY)
		{
			for (unsigned int i = 0; i < _maxclts; ++i)
				if (_clients[i].udpid >= 0)
					_udpsrv.write(_clients[i].udpid, msg->pckt.get_size(), msg->pckt.get_data());
		}
		else if (msg->type == Messagequeue::CONNECTION)
		{
			if (msg->cltid >= 0 && _clients[msg->cltid].tcp.is_good())
			{
				Header	hdr;
				hdr.size = sizeof(msg->actid);
				hdr.type = MID;
				_clients[msg->cltid].tcp.write(sizeof(hdr), &hdr);
				_clients[msg->cltid].tcp.write(hdr.size, &msg->actid);
			}
		}
		else if (msg->type == Messagequeue::DISCONNECTION)
			;//_delclient(id); KICK HIM
		else if (msg->type == Messagequeue::TEXTMSG)
		{
			hdr.size = msg->pckt.get_size();
			hdr.type = TEXTMSG;
			for (unsigned int i = 0; i < _maxclts; ++i)
			{
				if (_clients[i].tcp.is_good())
				{
					_clients[i].tcp.write(sizeof(hdr), &hdr);
					_clients[i].tcp.write(msg->pckt.get_size(), msg->pckt.get_data());
				}
			}
		}
		mq->pop_out();
	}
}
