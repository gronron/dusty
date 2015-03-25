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

#include <cmath>
#include <iostream>
#include "actormanager.hpp"
#include "console.hpp"
#include "graphicengine.hpp"
#include "renderer.hpp"
#include "uimanager.hpp"
#include "eventmanager.hpp"

#include <SFML/Window.hpp>

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
	sf::Event	event;

	if (am->graphic && am->ge->ready)
	{
		while (am->ge->_rdr->_window->pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					running = false;
					break;

				case sf::Event::Resized:
					am->ge->resize(event.size.width, event.size.height);
					am->um->resize(event.size.width, event.size.height);
					break;

				case sf::Event::LostFocus:
					break;

				case sf::Event::GainedFocus:
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

				case sf::Event::KeyPressed:
					_key(event, 1.0f);
					break;

				case sf::Event::KeyReleased:
					_key(event, 0.0f);
					break;

				case sf::Event::MouseWheelMoved:
					break;

				case sf::Event::MouseButtonPressed:
					_mousebutton(event, 1.0f);
					break;

				case sf::Event::MouseButtonReleased:
					_mousebutton(event, 0.0f);
					break;

				case sf::Event::MouseMoved:
					if (_mousemove.ctrl)
					{
						vec<float, 2>	a;

						a[0] = (float)event.mouseMove.x;
						a[1] = (float)event.mouseMove.y;
						(_mousemove.ctrl->*_mousemove.fx)(2, a.ar);
					}
					break;

				case sf::Event::MouseEntered:
					break;

				case sf::Event::MouseLeft:
					break;

				case sf::Event::JoystickButtonPressed:
					_joybutton(event, 1.0f);
					break;

				case sf::Event::JoystickButtonReleased:
					_joybutton(event, 0.0f);
					break;

				case sf::Event::JoystickMoved:
					_joymove(event);
					break;

				case sf::Event::JoystickConnected:
					break;

				case sf::Event::JoystickDisconnected:
					break;

				default:
					break;
			}
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

void		Eventmanager::_key(sf::Event &event, float d)
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

void				Eventmanager::_mousebutton(sf::Event &event, float d)
{
	Bind			*a;
	vec<float, 3>	b;

	b[0] = d;
	b[1] = (float)event.mouseButton.x;
	b[2] = (float)event.mouseButton.y;
	a = &_mousebuttonvector[event.mouseButton.button];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, (float *)&b);
}

void		Eventmanager::_joybutton(sf::Event &event, float d)
{
	Bind	*a;

	a = &_joybuttonvector[event.joystickButton.button];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, &d);
}

void	Eventmanager::_joymove(sf::Event &event)
{
	Bind	*a;
	float	b;

	b = event.joystickMove.position / 100.0f;
	a = &_joymovevector[event.joystickMove.axis];
	if (a->ctrl)
		(a->ctrl->*a->fx)(1, &b);
}
