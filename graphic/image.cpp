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

#include <iostream>
#include <cstdio>
#include <png.h>
#include "image.hpp"

Image::Image() : format(0), data(0)
{

}

Image::Image(std::string const &name) : format(0), data(0)
{
	load(name);
}

Image::~Image()
{
	if (data)
		delete [] data;
}

bool				Image::load(std::string const &name)
{
	FILE			*fp;
	png_byte		header[8];
	png_structp		png_ptr;
	png_infop		info_ptr;
	png_bytep		*row_pointers;
	int				bit_depth;
	int				color_type;

	if (data)
		delete [] data;

	if ((fp = fopen(name.c_str(), "rb")))
	{
		fread(header, 1, sizeof(header), fp);
		if (!png_sig_cmp(header, 0, 8))
		{
			if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
			{
				if ((info_ptr = png_create_info_struct(png_ptr)))
				{
					if (setjmp(png_jmpbuf(png_ptr)))
					{
						png_destroy_read_struct(&png_ptr, &info_ptr, 0);
						fclose(fp);
						return (false);
					}

					png_init_io(png_ptr, fp);
					png_set_sig_bytes(png_ptr, sizeof(header));
					png_read_info(png_ptr, info_ptr);

					bit_depth = png_get_bit_depth (png_ptr, info_ptr);

					png_get_IHDR (png_ptr, info_ptr, (png_uint_32 *)&size[0], (png_uint_32 *)&size[1], &bit_depth, &color_type, 0, 0, 0);

					if (color_type == PNG_COLOR_TYPE_PALETTE)
						png_set_palette_to_rgb(png_ptr);
					if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
						png_set_tRNS_to_alpha(png_ptr);
					if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
						png_set_expand_gray_1_2_4_to_8(png_ptr);
#ifdef	PNG_READ_16_TO_8_ACCURATE_SCALE
					if (bit_depth == 16)
						png_set_scale_16(png_ptr);
#endif
					if (bit_depth < 8)
						png_set_packing(png_ptr);

					png_read_update_info(png_ptr, info_ptr);

					color_type = png_get_color_type (png_ptr, info_ptr);

					if (color_type == PNG_COLOR_TYPE_GRAY)
						format = 1;
					else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
						format = 2;
					else if (color_type == PNG_COLOR_TYPE_RGB)
						format = 3;
					else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
						format = 4;
					else
					{
						png_destroy_read_struct(&png_ptr, &info_ptr, 0);
						fclose(fp);
						return (false);
					}

					data = new char[size[0] * size[1] * format];
					row_pointers = new png_bytep[size[1]];
					for (unsigned int i = 0; i < size[1]; ++i)
						row_pointers[i] = (unsigned char *)&data[(size[1] - i - 1) * size[0] * format];
					png_read_image(png_ptr, row_pointers);

					delete [] row_pointers;
					png_read_end(png_ptr, 0);
					png_destroy_read_struct(&png_ptr, &info_ptr, 0);
					fclose(fp);
					return (true);
				}
				else
					std::cerr << "Error: png_create_info_struct()" << std::endl;
				png_destroy_read_struct(&png_ptr, 0, 0);
			}
			else
				std::cerr << "Error: png_create_read_struct()" << std::endl;
		}
		else
			std::cerr << "Error: png_sig_cmp()" << std::endl;
		fclose(fp);
	}
	else
		std::cerr << "Error: Image::load() fails to open file: " << name << std::endl;
	return (false);
}
