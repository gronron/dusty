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
#include "time/time.hpp"
#include "actormanager.hpp"
#include "eventmanager.hpp"

bool	parse_option(char ac, char **av, Actormanager::Option &opt)
{
	opt.master = true;
	opt.local = true;
	opt.graphic = true;
	opt.audio = true;
	opt.level = "Level";

	for (int i = 1; i < ac; ++i)
	{
		if (!strcmp(av[i], "-ng"))
			opt.graphic = false;
		else if (!strcmp(av[i], "-na"))
			opt.audio = false;
		else if (opt.local)
		{
			opt.port = av[i];
			opt.local = false;
		}
		else if (opt.master)
		{
			opt.ip = av[i];
			opt.master = false;
		}
		else
			return (false);
	}
	return (true);
}

int			main(int ac, char **av)
{
	Actormanager::Option	opt;
	double	c;
	double	d;
	double	l;
	double	t = 0.0f;
	int		i = 0;

	if (!parse_option(ac, av, opt))
		return (-1);

	Actormanager	am(opt);

	c = src::clock();
	while (am.em->running)
	{
		l = c;
		d = (c = src::clock()) - l;
		am.tick((float)d);
		++i;
		if ((t += d) > 1.0f)
		{
			std::cout <<  i / t << std::endl;
			t = 0.0;
			i = 0;
		}
	}
	std::cout << "return" << std::endl;
	return (0);
}
