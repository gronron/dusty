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

#include <cmath>
#include <iostream>
#include "SDL.h"
#include "actormanager.hpp"
#include "console.hpp"
#include "graphicengine.hpp"
#include "renderer.hpp"
#include "uimanager.hpp"
#include "eventmanager.hpp"

Eventmanager::Eventmanager(Actormanager *a) : am(a), running(true), typing(false), _keyvector(sf::Keyboard::KeyCount), _mousebuttonvector(sf::Mouse::ButtonCount), _joybuttonvector(sf::Joystick::ButtonCount), _joymovevector(sf::Joystick::AxisCount)
{

}

Eventmanager::~Eventmanager()
{

}

void	Eventmanager::toogletyping()
{
	typing = !typing;
}

void			Eventmanager::event()
{
	SDL_Event	event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_APP_TERMINATING:
			case SDL_APP_LOWMEMORY:
			case SDL_APP_WILLENTERBACKGROUND:
			case SDL_APP_DIDENTERBACKGROUND:
			case SDL_APP_WILLENTERFOREGROUND:
			case SDL_APP_DIDENTERFOREGROUND:
			case SDL_WINDOWEVENT:
			case SDL_SYSWMEVENT:
				break;
			case SDL_KEYDOWN:
				_key(event, 1.0f);
				break;
			case SDL_KEYUP:
				_key(event, 0.0f);
				break;
			case SDL_TEXTEDITING:
			case SDL_TEXTINPUT:
			case SDL_MOUSEMOTION:
				if (_mousemove.ctrl)
				{
					vec<float, 2>	a;

					a[0] = (float)event.motion.x;
					a[1] = (float)event.motion.y;
					(_mousemove.ctrl->*_mousemove.fx)(2, a.ar);
				}
				event.motion
				break;
			case SDL_MOUSEBUTTONDOWN:
				_mousebutton(event, 1.0f);
				break;
			case SDL_MOUSEBUTTONUP:
				_mousebutton(event, 0.0f);
				break;
			case SDL_MOUSEWHEEL:
			case SDL_JOYAXISMOTION:
			case SDL_JOYBALLMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
				_joybutton(event, 1.0f);
				break;
			case SDL_JOYBUTTONUP:
				_joybutton(event, 0.0f);
				break;
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
			case SDL_CONTROLLERAXISMOTION:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_FINGERMOTION:
			case SDL_DOLLARGESTURE:
			case SDL_DOLLARRECORD:
			case SDL_MULTIGESTURE:
			case SDL_CLIPBOARDUPDATE:
			case SDL_DROPFILE:
			case SDL_RENDER_TARGETS_RESET:
			default:
				break;

			case sf::Event::Resized:
				am->ge->resize(event.size.width, event.size.height);
				am->um->resize(event.size.width, event.size.height);
				break;

			case sf::Event::TextEntered:
				if (typing)
				{
					std::cout << (int)event.text.unicode << std::endl;
					am->cl->putchar(event.text.unicode);
				}
				else if (event.text.unicode == '\n' || event.text.unicode == '\r')
					typing = true;
				break;
		}
	}
}

bool					Eventmanager::bind(std::string const &name, Controller *ctrl, Controller::BINDTYPE fx)
{
	unsigned long int	a;
	std::string			prefix;
	std::string			suffix;

	a = name.find('.');
	prefix = name.substr(0, a);
	if (a != std::string::npos)
		suffix = name.substr(a + 1);
	if (prefix == "key")
		;
	else if (prefix == "mouse_wheel")
		;
	else if (prefix == "mouse_button")
		;
	else if (prefix == "mouse_moved")
	{
		_mousemove.ctrl = ctrl;
		_mousemove.fx = fx;
	}
	else if (prefix == "joystick_button")
		;
	else if (prefix == "joystick_moved")
		;
	else
		std::cerr << "Warning: Eventmanager::bind() wrong bind name: " << name << std::endl;
	return (false);
}

void		Eventmanager::_key(SDL_Event &event, float d)
{
	Bind	*a;

	if (typing)
	{
		if (d)
		{
			switch (event.key.code)
			{
				case sf::Keyboard::Left:
					am->cl->movecursor(Console::LEFT);
					break;
				case sf::Keyboard::Right:
					am->cl->movecursor(Console::RIGHT);
					break;
				case sf::Keyboard::Up:
					am->cl->movecursor(Console::UP);
					break;
				case sf::Keyboard::Down:
					am->cl->movecursor(Console::DOWN);
					break;
			}
		}
	}
	else
	{
		a = &_keyvector[event.key.code];
		if (a->ctrl)
			(a->ctrl->*a->fx)(1, &d);
	}
}

void				Eventmanager::_mousebutton(SDL_Event &event, float d)
{
	Bind			*a;
	vec<float, 3>	b;

	b[0] = d;
	b[1] = (float)event.button.x;
	b[2] = (float)event.button.x;
	a = &_mousebuttonvector[event.button.button];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, (float *)&b);
}

void		Eventmanager::_joybutton(SDL_Event &event, float d)
{
	Bind	*a;

	a = &_joybuttonvector[event.joystickButton.button];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, &d);
}

void	Eventmanager::_joymove(SDL_Event &event)
{
	Bind	*a;
	float	b;

	b = event.joystickMove.position / 100.0f;
	a = &_joymovevector[event.joystickMove.axis];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, &b);
}
