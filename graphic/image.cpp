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
