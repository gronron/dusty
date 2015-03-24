///////////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010-2015 Geoffrey TOURON (geoffrey.touron@gmail.com)
//
//    This file is part of Dusty Engine.
//
//    Dusty Engine is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Dusty Engine is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Dusty Engine.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

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
	return (0);
}
