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
#include <cstdlib>
#include "messagequeue.hpp"
#include "client.hpp"

float const	Client::timeout = 5.0f;

Client::Client(Messagequeue *m, std::string const &ip, std::string const &port) : mq(m), _tcp(ip.c_str(), port.c_str()), _udp(ip.c_str(), port.c_str(), false), cntid(-1), ping(8), connected(false), timer(0.0f)
{
	Header	hdr;

	if (!_tcp.is_good() || !_udp.is_good())
		exit(EXIT_FAILURE);
	_slctr.add_socket(_tcp);
	_slctr.add_socket(_udp);
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
			hdr.size = msg->pckt.get_size();
			hdr.type = TEXTMSG;
			_tcp.write(sizeof(hdr), &hdr);
			_tcp.write(msg->pckt.get_size(), msg->pckt.get_data());
		}
		else
			_udp.write(msg->pckt.get_size(), msg->pckt.get_data());
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
