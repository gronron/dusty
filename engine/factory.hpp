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

#ifndef FACTORY_H_
#define FACTORY_H_

#include <map>
#include <string>

#define FACTORYREG(name) Factoryregister<name> const	reg_##name(#name)

class	Actor;
class	Actormanager;
class	Replication;

class	Factory
{
	public:

		typedef Actor *(*CF)(Actormanager *, Replication *, short int, int, Actor const *);

		struct	Class
		{
			short int	type;
			CF			cf;
		};


		static Factory	&get_instance();


		std::map<std::string, Class>	_classmap;


		void		register_class(std::string const &, CF);
		void		generate_type();

		short int	get_type(std::string const &) const;

		Actor		*create(Actormanager *, short int, Replication *) const;
		Actor		*create(Actormanager *, std::string const&, int, Actor const *) const;
};

template<class T>
Actor	*create(Actormanager *am, Replication *r, short int t, int i, Actor const *o)
{
	return (new T(am, r, t, i, o));
}

template<class T>
class	Factoryregister
{
	public:

		Factoryregister(std::string const &name)
		{
			Factory::get_instance().register_class(name, &create<T>);
		}
};

#endif
