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

Server::Server(Messagequeue *m, std::string const &port) : mq(m), _tcpsrv(port.c_str(), false), _udpsrv(port.c_str(), false), _currentid(0)
{
	if (!_tcpsrv.is_good() || !_udpsrv.is_good())
		exit(EXIT_FAILURE);
	_slctr.addsocket(_tcpsrv);
	_slctr.addsocket(_udpsrv);
}

Server::~Server()
{
	for (std::list<Client>::iterator i = _cltlist.begin(); i != _cltlist.end(); i = _cltlist.erase(i))
	{
		delete i->tcp;
		delete i->ping;
	}
}

void							Server::tick(float delta)
{
	std::list<Client>::iterator	i;

	_pingclient(delta);
	while (_slctr.check(0.0f))
	{
		if (_slctr.is_ready(_tcpsrv))
			_addclient();
		if (_slctr.is_ready(_udpsrv))
			_receivepacket();
		for (i = _cltlist.begin(); i != _cltlist.end();)
		{
			if (!_slctr.is_ready(*i->tcp) || _comtcpclient(i))
				++i;
		}
	}
	_sendpacket();
}

void		Server::_addclient()
{
	Client	clt;
	Header	hdr;

	clt.id = ++_currentid;
	clt.tcp = new Tcpstream(_tcpsrv, clt.ip, clt.port);
	if (!clt.tcp->is_good())
	{
		delete clt.tcp;
		return;
	}
	hdr.size = sizeof(clt.id);
	hdr.type = CNTID;
	if (clt.tcp->write(sizeof(Header), &hdr) < 0 || clt.tcp->write(hdr.size, &clt.id) < 0)
	{
		std::cerr << "Warning: Server::_addclient() fails to send connection id to: " << clt.port << " " << clt.ip << std::endl;
		delete clt.tcp;
		return;
	}
	clt.udpid = INVALID_SOCKET;
	clt.ping = new Ping(8);
	_slctr.addsocket(*clt.tcp);
	mq->push_in_cnt(clt.id);
	_cltlist.push_back(clt);
}

void	Server::_delclient(std::list<Client>::iterator	&i)
{
	std::cout << "Client disconected" << std::endl;
	mq->push_in_discnt(i->id);
	_udpsrv.rmclient(i->udpid);
	_slctr.rmsocket(*i->tcp);
	delete i->tcp;
	delete i->ping;
	i = _cltlist.erase(i);
}

void		Server::_pingclient(float delta)
{
	Header	hdr;

	for (std::list<Client>::iterator i = _cltlist.begin(); i != _cltlist.end();)
	{
		if  (i->ping->pinging)
		{
			if ((i->ping->tempping[i->ping->idx] += delta) > timeout)
				_delclient(i);
			else
				++i;
		}
		else
		{
			if ((i->ping->timer += delta) >= pingrate)
			{
				i->ping->start();
				hdr.type = FPING;
				i->tcp->write(sizeof(hdr), &hdr);
			}
			++i;
		}
	}
}

bool		Server::_comtcpclient(std::list<Client>::iterator	&i)
{
	Header	hdr;
	int		size;
	char	data[Udpstream::MAXUDPSIZE];

	if ((size = i->tcp->read(sizeof(hdr), &hdr)) > 0)
	{
		if (hdr.type == SPING)
		{
			hdr.type = TPING;
			i->tcp->write(sizeof(hdr), &hdr);
			i->ping->stop();
			return (true);
		}
		else if (hdr.type == TEXTMSG)
		{
			if ((size = i->tcp->read(hdr.size, data)) > 0)
			{
				mq->push_in_textmsg(i->id, size, data);
				return (true);
			}
		}
		else
			std::cerr << "Warning: Server::_comtcpclient() receives bad data from: " << i->port << " " << i->ip << std::endl;
	}
	_delclient(i);
	return (false);
}

void							Server::_receivepacket()
{
	std::list<Client>::iterator	i;
	char						a[Udpstream::MAXUDPSIZE];
	int							size;
	int							cltid;
	int							cltudpid;

	if ((size = _udpsrv.read(cltudpid, Udpstream::MAXUDPSIZE, a)) > 0)
	{
		if (cltudpid >= 0)
		{
			for (i = _cltlist.begin(); i != _cltlist.end(); ++i)
			{
				if (i->udpid == cltudpid)
				{
					mq->push_in_pckt(i->id, i->ping->ping, size, a);
					break;
				}
			}

		}
		else if (size == 4)
		{
			memcpy(&cltid, a, sizeof(long int));
			for (i = _cltlist.begin(); i != _cltlist.end(); ++i)
			{
				if (i->id == cltid)
				{
					i->udpid = _udpsrv.addclient();
					break;
				}
			}
		}
	}
	else if (size < 0)
		std::cerr << "Warning: Server::_receivepacket() fails to read on udp" << std::endl;
}

void							Server::_sendpacket()
{
	std::list<Client>::iterator	i;
	Messagequeue::Message		*msg;
	Header						hdr;

	while ((msg = mq->get_out()))
	{
		if (msg->type == Messagequeue::UPDATE || msg->type == Messagequeue::DESTROY)
		{
			for (i = _cltlist.begin(); i != _cltlist.end(); ++i)
				if (i->udpid >= 0)
					_udpsrv.write(i->udpid, msg->pckt->get_size(), msg->pckt->get_data());
			delete msg->pckt;
		}
		else if (msg->type == Messagequeue::CONNECTION)
		{
			for (i = _cltlist.begin(); i != _cltlist.end(); ++i)
			{
				if (i->id == msg->cltid)
				{
					Header	hdr;
					hdr.size = sizeof(msg->actid);
					hdr.type = MID;
					i->tcp->write(sizeof(hdr), &hdr);
					i->tcp->write(hdr.size, &msg->actid);
					break;
				}
			}
		}
		else if (msg->type == Messagequeue::DISCONNECTION)
			;//_delclient(id); KICK HIM
		else if (msg->type == Messagequeue::TEXTMSG)
		{
			hdr.size = msg->pckt->get_size();
			hdr.type = TEXTMSG;
			for (i = _cltlist.begin(); i != _cltlist.end(); ++i)
			{
				i->tcp->write(sizeof(hdr), &hdr);
				i->tcp->write(msg->pckt->get_size(), msg->pckt->get_data());
			}
			delete msg->pckt;
		}
		mq->pop_out();
	}
}
