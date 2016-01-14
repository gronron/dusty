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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"0
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

#ifndef OPENCL_ERROR
#define OPENCL_ERROR

#include <iostream>
#include <CL/cl.h>

inline char const	*error_to_string(cl_int error)
{
	switch (error)
	{
		case 0:		return "CL_SUCCESS";
		case -1:	return "CL_DEVICE_NOT_FOUND";
		case -2:	return "CL_DEVICE_NOT_AVAILABLE";
		case -3:	return "CL_COMPILER_NOT_AVAILABLE";
		case -4:	return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case -5:	return "CL_OUT_OF_RESOURCES";
		case -6:	return "CL_OUT_OF_HOST_MEMORY";
		case -7:	return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case -8:	return "CL_MEM_COPY_OVERLAP";
		case -9:	return "CL_IMAGE_FORMAT_MISMATCH";
		case -10:	return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case -11:	return "CL_BUILD_PROGRAM_FAILURE";
		case -12:	return "CL_MAP_FAILURE";
		case -13:	return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
		case -14:	return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
		case -15:	return "CL_COMPILE_PROGRAM_FAILURE";
		case -16:	return "CL_LINKER_NOT_AVAILABLE";
		case -17:	return "CL_LINK_PROGRAM_FAILURE";
		case -18:	return "CL_DEVICE_PARTITION_FAILED";
		case -19:	return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

		case -30:	return "CL_INVALID_VALUE";
		case -31:	return "CL_INVALID_DEVICE_TYPE";
		case -32:	return "CL_INVALID_PLATFORM";
		case -33:	return "CL_INVALID_DEVICE";
		case -34:	return "CL_INVALID_CONTEXT";
		case -35:	return "CL_INVALID_QUEUE_PROPERTIES";
		case -36:	return "CL_INVALID_COMMAND_QUEUE";
		case -37:	return "CL_INVALID_HOST_PTR";
		case -38:	return "CL_INVALID_MEM_OBJECT";
		case -39:	return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case -40:	return "CL_INVALID_IMAGE_SIZE";
		case -41:	return "CL_INVALID_SAMPLER";
		case -42:	return "CL_INVALID_BINARY";
		case -43:	return "CL_INVALID_BUILD_OPTIONS";
		case -44:	return "CL_INVALID_PROGRAM";
		case -45:	return "CL_INVALID_PROGRAM_EXECUTABLE";
		case -46:	return "CL_INVALID_KERNEL_NAME";
		case -47:	return "CL_INVALID_KERNEL_DEFINITION";
		case -48:	return "CL_INVALID_KERNEL";
		case -49:	return "CL_INVALID_ARG_INDEX";
		case -50:	return "CL_INVALID_ARG_VALUE";
		case -51:	return "CL_INVALID_ARG_SIZE";
		case -52:	return "CL_INVALID_KERNEL_ARGS";
		case -53:	return "CL_INVALID_WORK_DIMENSION";
		case -54:	return "CL_INVALID_WORK_GROUP_SIZE";
		case -55:	return "CL_INVALID_WORK_ITEM_SIZE";
		case -56:	return "CL_INVALID_GLOBAL_OFFSET";
		case -57:	return "CL_INVALID_EVENT_WAIT_LIST";
		case -58:	return "CL_INVALID_EVENT";
		case -59:	return "CL_INVALID_OPERATION";
		case -60:	return "CL_INVALID_GL_OBJECT";
		case -61:	return "CL_INVALID_BUFFER_SIZE";
		case -62:	return "CL_INVALID_MIP_LEVEL";
		case -63:	return "CL_INVALID_GLOBAL_WORK_SIZE";
		case -64:	return "CL_INVALID_PROPERTY";
		case -65:	return "CL_INVALID_IMAGE_DESCRIPTOR";
		case -66:	return "CL_INVALID_COMPILER_OPTIONS";
		case -67:	return "CL_INVALID_LINKER_OPTIONS";
		case -68:	return "CL_INVALID_DEVICE_PARTITION_COUNT";

		case -1000:	return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
		case -1001:	return "CL_PLATFORM_NOT_FOUND_KHR";
		case -1002:	return "CL_INVALID_D3D10_DEVICE_KHR";
		case -1003:	return "CL_INVALID_D3D10_RESOURCE_KHR";
		case -1004:	return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
		case -1005:	return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
		default:	return "Unknown OpenCL error";
	}
}

inline void	check_error(cl_int error, char const *str)
{
	if (error != CL_SUCCESS)
	{
		std::cerr << str << ": " << error_to_string(error) << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
}

#endif
