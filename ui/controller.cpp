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

#include "endian/packet.hpp"
#include "replication.hpp"
#include "actormanager.hpp"
#include "controller.hpp"

Controller::Controller(Actormanager *a, Replication *r, short int t, int i, Actor const *o) : Actor(a, r, t, i, o)
{
	if (!am->local && !rp)
		rp = new Replication(this, 0.2f, 5.0f);
}

Controller::~Controller()
{

}

void	Controller::postinstanciation()
{
	if (am->_controllermap.find(id) != am->_controllermap.end())
		bind();
}

void	Controller::bind()
{
	am->_controllermap[id] = this;
	if (!am->local)
		rp->master = true;
}
