/******************************************************************************
Copyright (c) 2015-2017, Geoffrey TOURON
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

#pragma once

#include "math/vec.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

class	Graphicengine;

struct	SDL_Window;

struct	Glyph
{
	vec<float, 2>	size;
	vec<float, 2>	topleft;
	vec<float, 2>	bottomright;
	vec<float, 2>	step;
	vec<float, 2>	center;
};

class	Renderer
{
	public:

		unsigned int	width;
		unsigned int	height;

		SDL_Window		*_window;

		VkInstance		_instance;
		VkDevice		_device;
		VkSurfaceKHR	_surface;


		Renderer(unsigned int const, unsigned int const, bool const);
		~Renderer();

		void			set_fullscreen(bool const);
		void			set_resolution(unsigned int const, unsigned int const);
		void			draw_text(char const *, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &) const {};
		void			draw_text(unsigned int const, char const *, vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &) const {};
		unsigned int	cut_line(char const *text, vec<float, 2> const &, float const) const { return (unsigned int)strlen(text); };

		void			render(Graphicengine const *);

		void			_draw_glyph(vec<float, 2> const &, vec<float, 2> const &, vec<float, 4> const &, const unsigned int) {};
		void			_set_buffer(Graphicengine const *);

		void			_create_vk_instance();
		void			_create_vk_device();
		void			_create_vk_surface();
};
