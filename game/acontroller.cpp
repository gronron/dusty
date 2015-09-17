#include "common.hpp"
#include "eventmanager.hpp"
#include "acontroller.hpp"
#include "SDL.h"
#include <iostream>

FACTORYREG(AController);

AController::AController(Gameengine *g, Replication *r, int i, short int t, Actor const *o) : Controller(g, r, i, t, o)
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
	if (engine->master)
		controlled = (Player *)engine->create("Player", this, true);
	if (!engine->master)
		engine->notify_owned(this, true);
}

void	AController::notified_by_owned(Actor *a, bool l)
{
	if (l)
	{
		controlled = (Player *)a;
		if (engine->graphic && engine->_controllermap.find(id) != engine->_controllermap.end())
		{
			//am->ge->add(controlled->hud);
		}
	}
	else
		controlled = 0;
}

void	AController::get_replication(Packet &pckt) const
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
		//controlled->body->velocity = move * 256.0f;
		controlled->firing = firing;
		controlled->loadingfire = loadingfire;
		controlled->dir = aim;
		//beware
		
		vec<float, 4>	direction;
		vec<float, 4>	right;
	
		direction[0] = cos(engine->graphic->camera.spherical_coord[1]) * cos(engine->graphic->camera.spherical_coord[0]);
		direction[1] = cos(engine->graphic->camera.spherical_coord[1]) * sin(engine->graphic->camera.spherical_coord[0]);
		direction[2] = sin(engine->graphic->camera.spherical_coord[1]);
		direction[3] = 0.0f;
		
		right[0] = sin(engine->graphic->camera.spherical_coord[0]);
		right[1] = -cos(engine->graphic->camera.spherical_coord[0]);
		right[2] = 0.0f;
		right[3] = 0.0f;
	
		controlled->body->velocity = (direction * -move[1] + right * move[0]) * delta * 1000.0f;
		engine->graphic->camera.position = controlled->body->position;
	}
}

void	AController::bind()
{
	Controller::bind();
	if (engine->graphic && controlled)
	{
		//am->ge->add(controlled->hud);
	}
	engine->event->_keys[SDL_SCANCODE_E].ctrl = this;
	engine->event->_keys[SDL_SCANCODE_E].fx = (BINDTYPE)&AController::forward;

	engine->event->_keys[SDL_SCANCODE_D].ctrl = this;
	engine->event->_keys[SDL_SCANCODE_D].fx = (BINDTYPE)&AController::backward;

	engine->event->_keys[SDL_SCANCODE_S].ctrl = this;
	engine->event->_keys[SDL_SCANCODE_S].fx = (BINDTYPE)&AController::left;

	engine->event->_keys[SDL_SCANCODE_F].ctrl = this;
	engine->event->_keys[SDL_SCANCODE_F].fx = (BINDTYPE)&AController::right;

	engine->event->_mousemove.ctrl = this;
	engine->event->_mousemove.fx = (BINDTYPE)&AController::aimloc;

	engine->event->_mousebuttons[SDL_BUTTON_LEFT].ctrl = this;
	engine->event->_mousebuttons[SDL_BUTTON_LEFT].fx = (BINDTYPE)&AController::fire;

	engine->event->_mousebuttons[SDL_BUTTON_RIGHT].ctrl = this;
	engine->event->_mousebuttons[SDL_BUTTON_RIGHT].fx = (BINDTYPE)&AController::strongfire;
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
		//aim = unit<float>(aim - vec<float, 4>::cast(engine->graphic->screensize) / 2.0f);
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
		aim[0] += data[0] / 1000.0f;
		aim[1] += data[1] / 1000.0f;
		
		//aim = unit<float>(aim - vec<float, 4>::cast(engine->graphic->screensize) / 2.0f);
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
