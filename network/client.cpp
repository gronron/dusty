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

#include <iostream>
#include <cstdlib>
#include "messagequeue.hpp"
#include "client.hpp"

float const	Client::timeout = 5.0f;

Client::Client(Messagequeue *m, std::string const &ip, std::string const &port) : mq(m), _tcp(ip.c_str(), port.c_str()), _udp(ip.c_str(), port.c_str(), false), cntid(-1), ping(8), connected(false), timer(0.0f)
{
	Header	hdr;

	if (!_tcp.is_good() || !_udp.is_good())
		exit(EXIT_FAILURE);
	_slctr.addsocket(_tcp);
	_slctr.addsocket(_udp);
	_tcp.read(sizeof(Header), &hdr);
	if (hdr.type != CNTID || hdr.size != sizeof(cntid) || hdr.size != _tcp.read(hdr.size, &cntid))
	{
		std::cerr << "Error: Client::Client() fails to receive connection id." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Connected" << std::endl;
}

Client::~Client()
{

}

void						Client::tick(float delta)
{
	char					a[Udpstream::MAXUDPSIZE];
	int						size;
	Messagequeue::Message	*msg;
	Header					hdr;

	if (ping.pinging)
		ping.tempping[ping.idx] += delta;
	while (_slctr.check(0))
	{
		if (_slctr.is_ready(_udp))
		{
			if (!connected)
				connected = true;
			if ((size = _udp.read(Udpstream::MAXUDPSIZE, a)) > 0)
				mq->push_in_pckt(-1, ping.ping, size, a);
			else if (size < 0)
				std::cerr << "Error: Client::tick() fails to read on udp" << std::endl;
		}
		if (_slctr.is_ready(_tcp))
			_comtcp();
	}

	while ((msg = mq->get_out()))
	{
		if (msg->type == Messagequeue::TEXTMSG)
		{
			hdr.size = msg->pckt->getsize();
			hdr.type = TEXTMSG;
			_tcp.write(sizeof(hdr), &hdr);
			_tcp.write(msg->pckt->getsize(), msg->pckt->getdata());
		}
		else
			_udp.write(msg->pckt->getsize(), msg->pckt->getdata());
		delete msg->pckt;
		mq->pop_out();
	}
	if (!connected && (timer += delta) >= 0.1f)
	{
		timer = 0.0f;
		_udp.write(sizeof(cntid), &cntid);
	}
}

void		Client::_comtcp()
{
	Header	hdr;
	int		size;
	int		id;
	char	data[Udpstream::MAXUDPSIZE];

	if ((size = _tcp.read(sizeof(hdr), &hdr)) > 0)
	{
		if (hdr.type == FPING)
		{
			hdr.type = SPING;
			_tcp.write(sizeof(hdr), &hdr);
			ping.start();
		}
		else if (hdr.type == TPING)
		{
			ping.stop();
		}
		else if (hdr.type == MID)
		{
			if ((size = _tcp.read(hdr.size, &id)) > 0)
				mq->push_in_cntrl(id);
			else
			{
				std::cout << "Server disconnected" << std::endl;
				exit(EXIT_SUCCESS);
			}
		}
		else if (hdr.type == TEXTMSG)
		{
			if ((size = _tcp.read(hdr.size, data)) > 0)
				mq->push_in_textmsg(size, data);
			else
			{
				std::cout << "Server disconnected" << std::endl;
				exit(EXIT_SUCCESS);
			}
		}
		else
			std::cerr << "Warning: Client::_comtcp() receives bad data" << std::endl;
	}
	else
	{
		std::cout << "Server disconnected" << std::endl;
		exit(EXIT_SUCCESS);
	}
}
