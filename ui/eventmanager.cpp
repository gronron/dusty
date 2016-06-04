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

#include "gameengine.hpp"
#include "console.hpp"
#include "graphicengine.hpp"
#include "eventmanager.hpp"

Eventmanager::Eventmanager(Gameengine *e) : engine(e), running(true), typing(false)
{

}

Eventmanager::~Eventmanager()
{

}

void			Eventmanager::event()
{
	SDL_Event	event;

	SDL_StartTextInput();
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
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					engine->graphic->set_resolution(event.window.data1, event.window.data2);
				else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
					SDL_SetRelativeMouseMode(SDL_TRUE);
				else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
					SDL_SetRelativeMouseMode(SDL_FALSE);
				break;
			case SDL_SYSWMEVENT:
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					typing = !typing;
				else
				{
					if (typing)
						_type(event);
					else
						_key(event, 1.0f);
				}
				break;
			case SDL_KEYUP:
				if (!typing)
					_key(event, 0.0f);
				break;
			case SDL_TEXTEDITING:
				break;
			case SDL_TEXTINPUT:
				if (typing)
					engine->console->put_char(*event.text.text);
				break;
			case SDL_MOUSEMOTION:
				if (_mousemove.ctrl)
				{
					vec<float, 2>	a;

					a[0] = (float)event.motion.x;
					a[1] = (float)event.motion.y;
					(_mousemove.ctrl->*_mousemove.fx)(2, a.ar);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				_mousebutton(event, 1.0f);
				break;
			case SDL_MOUSEBUTTONUP:
				_mousebutton(event, 0.0f);
				break;
			case SDL_MOUSEWHEEL:
				if (_mousewheel.ctrl)
				{
					float	a;

					a = (float)event.wheel.y;
					(_mousewheel.ctrl->*_mousewheel.fx)(1, &a);
				}
				break;
			case SDL_JOYAXISMOTION:
			case SDL_JOYBALLMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
				break;
			case SDL_CONTROLLERAXISMOTION:
				_gamepadmove(event);
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				_gamepadbutton(event, 1.0f);
				break;
			case SDL_CONTROLLERBUTTONUP:
				_gamepadbutton(event, 0.0f);
				break;
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
				break;
			case SDL_DROPFILE:
				SDL_free(event.drop.file);
				break;
			case SDL_RENDER_TARGETS_RESET:
			default:
				break;
		}
	}
}

void	Eventmanager::_type(SDL_Event &event)
{
	switch (event.key.keysym.scancode)
	{
		case SDL_SCANCODE_RETURN:
			engine->console->put_char('\n');
			break;
		case SDL_SCANCODE_BACKSPACE:
			engine->console->put_char('\b');
			break;
		case SDL_SCANCODE_TAB:
			engine->console->put_char('\t');
			break;
		case SDL_SCANCODE_DELETE:
			engine->console->put_char(0x7f);
			break;
		case SDL_SCANCODE_RIGHT:
			engine->console->move_cursor(Console::RIGHT);
			break;
		case SDL_SCANCODE_LEFT:
			engine->console->move_cursor(Console::LEFT);
			break;
		case SDL_SCANCODE_DOWN:
			engine->console->move_cursor(Console::DOWN);
			break;
		case SDL_SCANCODE_UP:
			engine->console->move_cursor(Console::UP);
			break;
	}
}

void		Eventmanager::_key(SDL_Event &event, float d)
{
	Bind	*a;

	a = &_keys[event.key.keysym.scancode];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, &d);
}

void				Eventmanager::_mousebutton(SDL_Event &event, float d)
{
	Bind			*a;
	vec<float, 3>	b;

	b[0] = d;
	b[1] = (float)event.button.x;
	b[2] = (float)event.button.y;
	a = &_mousebuttons[event.button.button];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, (float *)&b);
}

void		Eventmanager::_gamepadbutton(SDL_Event &event, float d)
{
	Bind	*a;

	a = &_gamepadbuttons[event.cbutton.button];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, &d);
}

void	Eventmanager::_gamepadmove(SDL_Event &event)
{
	Bind	*a;
	float	b;

	b = event.caxis.value / 32767.0f;
	a = &_gamepadmoves[event.caxis.axis];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, &b);
}
