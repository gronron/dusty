#include "common.hpp"
#include "eventmanager.hpp"
#include "acontroller.hpp"
#include "SDL.h"
#include <iostream>
#include "world.hpp"

FACTORYREG(AController);

AController::AController(Gameengine *g, Replication *r, int i, short int t, Entity const *o) : Controller(g, r, i, t, o)
{
	controllertype = SPECTATOR;
	controlled = 0;
	firing = false;
	loadingfire = false;
	aim = 0.0f;
	move = 0.0f;
	material = 1;
	spherical_coord = 0.0f;
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
		controlled = (Player *)engine->create(CRC32("Player"), this, true);
	if (!engine->master)
		engine->notify_owned(this, true);
}

void	AController::notified_by_owned(Entity *e, bool l)
{
	Controller::notified_by_owned(e, l);
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
		int	x;
		int	y;
		SDL_GetRelativeMouseState(&x, &y);
		spherical_coord[0] -= x * 0.0016f;
		spherical_coord[1] -= y * 0.0016f;

		engine->graphic->set_camera(controlled->body->position, spherical_coord);
	
		controlled->body->velocity = (engine->graphic->camera.direction * move[1] + engine->graphic->camera.right * move[0]) * 8.0f;

		controlled->firing = firing;
		controlled->loadingfire = loadingfire;
		controlled->dir = engine->graphic->camera.direction;
		
		if (controllertype == WORLDCREATOR)
		{
			//draw the shit
		}
	}
}

void	AController::change_type(char const tp)
{
	controllertype = tp;
	if (rp)
		rp->needupdate = true;
	if (!rp || rp->authority & Replication::LOCAL)
		bind();
}

void	AController::bind()
{
	Controller::bind();
	if (engine->graphic && controlled)
	{
		//am->ge->add(controlled->hud);
	}

	engine->event->_keys[SDL_SCANCODE_F1].ctrl = this;
	engine->event->_keys[SDL_SCANCODE_F1].fx = (BINDTYPE)&AController::switch_to_spectator;
	
	engine->event->_keys[SDL_SCANCODE_F2].ctrl = this;
	engine->event->_keys[SDL_SCANCODE_F2].fx = (BINDTYPE)&AController::switch_to_worldcreator;
	
	engine->event->_keys[SDL_SCANCODE_F3].ctrl = this;
	engine->event->_keys[SDL_SCANCODE_F3].fx = (BINDTYPE)&AController::switch_to_fighter;
	
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

	if (controllertype == SPECTATOR)
	{
		engine->event->_mousebuttons[SDL_BUTTON_LEFT].ctrl = 0;
		engine->event->_mousebuttons[SDL_BUTTON_RIGHT].ctrl = 0;
		engine->event->_mousewheel.ctrl = 0;
		engine->event->_keys[SDL_SCANCODE_L].ctrl = 0;
		engine->event->_keys[SDL_SCANCODE_M].ctrl = 0;
	}
	else if (controllertype == WORLDCREATOR)
	{
		engine->event->_mousebuttons[SDL_BUTTON_LEFT].ctrl = this;
		engine->event->_mousebuttons[SDL_BUTTON_LEFT].fx = (BINDTYPE)&AController::create_block;

		engine->event->_mousebuttons[SDL_BUTTON_RIGHT].ctrl = this;
		engine->event->_mousebuttons[SDL_BUTTON_RIGHT].fx = (BINDTYPE)&AController::destroy_block;

		engine->event->_mousewheel.ctrl = this;
		engine->event->_mousewheel.fx = (BINDTYPE)&AController::change_material;
	}
	else if (controllertype == FIGHTER)
	{
		engine->event->_mousebuttons[SDL_BUTTON_LEFT].ctrl = this;
		engine->event->_mousebuttons[SDL_BUTTON_LEFT].fx = (BINDTYPE)&AController::fire;

		engine->event->_mousebuttons[SDL_BUTTON_RIGHT].ctrl = this;
		engine->event->_mousebuttons[SDL_BUTTON_RIGHT].fx = (BINDTYPE)&AController::strongfire;
		
		engine->event->_mousewheel.ctrl = 0;
		engine->event->_keys[SDL_SCANCODE_L].ctrl = 0;
		engine->event->_keys[SDL_SCANCODE_M].ctrl = 0;
	}
}

//// SPECTATOR ////////////////////////

void	AController::switch_to_spectator(int const size, float const *data)
{
	if (size == 1 && !*data)
		change_type(SPECTATOR);
}

void	AController::switch_to_worldcreator(int const size, float const *data)
{
	if (size == 1 && !*data)
		change_type(WORLDCREATOR);
}

void	AController::switch_to_fighter(int const size, float const *data)
{
	if (size == 1 && !*data)
		change_type(FIGHTER);
}

void	AController::forward(int const size, float const *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[1] > 0.0f))
			move[1] = *data;
	}
}

void	AController::backward(int const size, float const *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[1] < 0.0f))
			move[1] = -*data;
	}
}

void	AController::left(int const size, float const *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[0] < 0.0f))
			move[0] = -*data;
	}
}

void	AController::right(int const size, float const *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		if (*data > 0.0f || (*data == 0.0f && move[0] > 0.0f))
			move[0] = *data;
	}
}

void	AController::aimloc(int const size, float const *data)
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

//// MAPCREATOR ///////////////////////

void	AController::create_block(int const size, float const *data)
{
	if (size == 1)
	{
		if (*data && controlled)
		{
			Ray	ray;
			
			ray.origin = engine->graphic->camera.position;
			ray.direction = engine->graphic->camera.direction;

			((World*)engine->find_entity(0))->create_block(ray, material);
		}
	}
}

void	AController::destroy_block(int const size, float const *data)
{
	if (size == 1)
	{
		if (*data && controlled)
		{
			Ray	ray;

			ray.origin = engine->graphic->camera.position;
			ray.direction = engine->graphic->camera.direction;

			((World*)engine->find_entity(0))->destroy_block(ray);
		}
	}
}

void	AController::change_material(int const size, float const *data)
{
	if (size == 1)
	{
		std::cout << engine->graphic->_materials.number << std::endl;
		material = (material + (int)*data) % engine->graphic->_materials.number;
		if (!material)
			material = 1;
	}
}

//// FIGHTER //////////////////////////

void	AController::fire(int const size, float const *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		firing = *data;
	}
}

void	AController::strongfire(int const size, float const *data)
{
	if (size == 1)
	{
		if (rp)
			rp->needupdate = true;
		loadingfire = *data;
	}
}
