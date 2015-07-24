#include "math/vec_util.hpp"
#include "endian/packet.hpp"
#include "replication.hpp"
#include "factory.hpp"
#include "actormanager.hpp"
#include "eventmanager.hpp"
#include "acontroller.hpp"
#include "graphicengine.hpp"
#include "SDL.h"
#include <iostream>

FACTORYREG(AController);

AController::AController(Actormanager *a, Replication *r, int i, short int t, Actor const *o) : Controller(a, r, i, t, o)
{
	controlled = 0;
	firing = false;
	loadingfire = false;
	aim = 0.0f;
	move = 0.0f;
}

AController::~AController()
{
	if (controlled)
		controlled->destroy();
}

void	AController::postinstanciation()
{
	Controller::postinstanciation();
	if (am->master)
		controlled = (Player *)am->create("Player", this, true);
	if (!am->master)
		am->notify_owned(this, true);
}

void	AController::notified_by_owned(Actor *a, bool l)
{
	if (l)
	{
		controlled = (Player *)a;
		if (am->graphic && am->_controllermap.find(id) != am->_controllermap.end())
		{
			//am->ge->set_camera(&controlled->bd.loc);
			//am->ge->add(controlled->hud);
		}
	}
	else
		controlled = 0;
}

void	AController::get_replication(Packet &pckt)
{
	Controller::get_replication(pckt);
	pckt.write(firing);
	pckt.write(aim);
	pckt.write(move);
}

void	AController::replicate(Packet &pckt, float p)
{
	if (rp->authority == Replication::NONE)
	{
		Controller::replicate(pckt, p);
		pckt.read(firing);
		pckt.read(aim);
		pckt.read(move);
		if (controlled)
			controlled->rp->needupdate = true;
	}
}

void	AController::tick(float delta)
{
	Controller::tick(delta);
	if (controlled)
	{
		controlled->bdb->nextspd = move * 256.0f;
		controlled->firing = firing;
		controlled->loadingfire = loadingfire;
		controlled->dir = aim;
		//beware
		am->ge->cam_loc = controlled->bdb->loc;
	}
}

void	AController::bind()
{
	Controller::bind();
	if (am->graphic && controlled)
	{
		//am->ge->add(controlled->hud);
	}
	am->em->_keys[SDL_SCANCODE_E].ctrl = this;
	am->em->_keys[SDL_SCANCODE_E].fx = (BINDTYPE)&AController::forward;

	am->em->_keys[SDL_SCANCODE_D].ctrl = this;
	am->em->_keys[SDL_SCANCODE_D].fx = (BINDTYPE)&AController::backward;

	am->em->_keys[SDL_SCANCODE_S].ctrl = this;
	am->em->_keys[SDL_SCANCODE_S].fx = (BINDTYPE)&AController::left;

	am->em->_keys[SDL_SCANCODE_F].ctrl = this;
	am->em->_keys[SDL_SCANCODE_F].fx = (BINDTYPE)&AController::right;

	am->em->_mousemove.ctrl = this;
	am->em->_mousemove.fx = (BINDTYPE)&AController::aimloc;

	am->em->_mousebuttons[SDL_BUTTON_LEFT].ctrl = this;
	am->em->_mousebuttons[SDL_BUTTON_LEFT].fx = (BINDTYPE)&AController::fire;

	am->em->_mousebuttons[SDL_BUTTON_RIGHT].ctrl = this;
	am->em->_mousebuttons[SDL_BUTTON_RIGHT].fx = (BINDTYPE)&AController::strongfire;

/*
	am->em->_joymovevector[0].ctrl = this;
	am->em->_joymovevector[0].fx = (BINDTYPE)&AController::movex;

	am->em->_joymovevector[1].ctrl = this;
	am->em->_joymovevector[1].fx = (BINDTYPE)&AController::movey;

	am->em->_joymovevector[2].ctrl = this;
	am->em->_joymovevector[2].fx = (BINDTYPE)&AController::aimdirx;

	am->em->_joymovevector[3].ctrl = this;
	am->em->_joymovevector[3].fx = (BINDTYPE)&AController::aimdiry;

	am->em->_joybuttonvector[5].ctrl = this;
	am->em->_joybuttonvector[5].fx = (BINDTYPE)&AController::fire;*/
}

void	AController::forward(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[1] < 0.0f))
			move[1] = -*data;
		
	}
}

void	AController::backward(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[1] > 0.0f))
			move[1] = *data;
	}
}

void	AController::left(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[0] < 0.0f))
			move[0] = -*data;
	}
}

void	AController::right(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[0] > 0.0f))
			move[0] = *data;
	}
}

void	AController::movex(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		move[0] = *data;
	}
}

void	AController::movey(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		move[1] = *data;
	}
}

void	AController::fire(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		firing = *data;
		aim[0] = data[1];
		aim[1] = data[2];
		aim = Sgl::unit(aim - vec<float, 4>::cast(am->ge->screensize) / 2.0f);
	}
}

void	AController::strongfire(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		loadingfire = *data;
	}
}

void	AController::aimloc(int size, float *data)
{
	if (size == 2)
	{
		if (rp)
			rp->needupdate = true;
		aim[0] = data[0];
		aim[1] = data[1];
		aim = Sgl::unit(aim - vec<float, 4>::cast(am->ge->screensize) / 2.0f);
	}
}

void	AController::aimdirx(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		aim[0] = *data;
	}
}

void	AController::aimdiry(int size, float *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		aim[1] = *data;
	}
}
