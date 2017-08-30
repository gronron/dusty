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

#include <SDL.h>
#include <SDL_syswm.h>
#include <iostream>
#include <cstdlib>
#include "new.hpp"
#include "math/vec_util.hpp"
#include "file/file.hpp"
#include "graphicengine.hpp"
#include "renderer_vk.hpp"
#include <vulkan/vk_sdk_platform.h>

#include <vector>

struct				Computedcamera
{
	vec<float, 4>	position;
	vec<float, 4>	forward;
	vec<float, 4>	right;
	vec<float, 4>	up;
	vec<float, 2>	half_resolution;
};

void	check_vk_error(VkResult const result, char const* msg)
{
	if (result != VK_SUCCESS)
	{
		std::cerr << "error! " << msg << " : ";
		switch (result)
		{
			case VK_NOT_READY:						std::cerr << "VK_NOT_READY"; break;
			case VK_TIMEOUT:						std::cerr << "VK_TIMEOUT"; break;
			case VK_EVENT_SET:						std::cerr << "VK_EVENT_SET"; break;
			case VK_EVENT_RESET:					std::cerr << "VK_EVENT_RESET"; break;
			case VK_INCOMPLETE:						std::cerr << "VK_INCOMPLETE"; break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:		std::cerr << "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:		std::cerr << "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
			case VK_ERROR_INITIALIZATION_FAILED:	std::cerr << "VK_ERROR_INITIALIZATION_FAILED"; break;
			case VK_ERROR_DEVICE_LOST:				std::cerr << "VK_ERROR_DEVICE_LOST"; break;
			case VK_ERROR_MEMORY_MAP_FAILED:		std::cerr << "VK_ERROR_MEMORY_MAP_FAILED"; break;
			case VK_ERROR_LAYER_NOT_PRESENT:		std::cerr << "VK_ERROR_LAYER_NOT_PRESENT"; break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:	std::cerr << "VK_ERROR_EXTENSION_NOT_PRESENT"; break;
			case VK_ERROR_FEATURE_NOT_PRESENT:		std::cerr << "VK_ERROR_FEATURE_NOT_PRESENT"; break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:		std::cerr << "VK_ERROR_INCOMPATIBLE_DRIVER"; break;
			case VK_ERROR_TOO_MANY_OBJECTS:			std::cerr << "VK_ERROR_TOO_MANY_OBJECTS"; break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:		std::cerr << "VK_ERROR_FORMAT_NOT_SUPPORTED"; break;
			case VK_ERROR_FRAGMENTED_POOL:			std::cerr << "VK_ERROR_FRAGMENTED_POOL"; break;
			case VK_ERROR_SURFACE_LOST_KHR:			std::cerr << "VK_ERROR_SURFACE_LOST_KHR"; break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:	std::cerr << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; break;
			case VK_SUBOPTIMAL_KHR:					std::cerr << "VK_SUBOPTIMAL_KHR"; break;
			case VK_ERROR_OUT_OF_DATE_KHR:			std::cerr << "VK_ERROR_OUT_OF_DATE_KHR"; break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:	std::cerr << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; break;
			case VK_ERROR_VALIDATION_FAILED_EXT:	std::cerr << "VK_ERROR_VALIDATION_FAILED_EXT"; break;
			default:								std::cerr << "UNKNOW_ERROR"; break;
		}
		std::cerr << std::endl;
	    exit(EXIT_FAILURE);
	}
}

void	_compute_camera(Camera const &camera, Computedcamera &cm)
{
	cm.position = camera.position;
	cm.forward = (camera.direction * (float)camera.resolution[0]) / (tan(camera.fov / 2.0f) * 2.0f);
	cm.right = camera.right;
	cm.up = vunit<float>(vcross(camera.direction, camera.right));
	cm.half_resolution = (vec<float, 2>)camera.resolution / 2.0f;
}

unsigned int	_load_font(char const *, Glyph *);

Renderer::Renderer(unsigned int const w, unsigned int const h, bool const fullscreen) : width(w), height(h), _window(0)
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "error! SDL_Init()" << std::endl;
		exit(EXIT_FAILURE);
	}

	_window = SDL_CreateWindow("dusty", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

	/*uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
	vector<VkQueueFamilyProperties> familyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount &damilyProperties[0]);

	// Print the families
	for (uint32_t i = 0; i < deviceCount; i++) {
	    for (uint32_t j = 0; j < queueFamilyCount; j++) {
	        printf("Count of Queues: %d\n", familyProperties[j].queueCount);
	        printf("Supported operationg on this queue:\n");
	        if (familyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
	            printf("\t\t Graphics\n");
	        if (familyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
	            printf("\t\t Compute\n");
	        if (familyProperties[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
	            printf("\t\t Transfer\n");
	        if (familyProperties[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
	            printf("\t\t Sparse Binding\n");
	    }
	}*/

	_create_vk_instance();
	_create_vk_device();
	_create_vk_surface();
}

Renderer::~Renderer()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);

	vkDestroyInstance(_instance, nullptr);

	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void	Renderer::_create_vk_instance()
{
	VkInstanceCreateInfo		instance_info = {};
	VkApplicationInfo			app_info = {};
	std::vector<char const *>	extensiosn = {VK_KHR_SURFACE_EXTENSION_NAME , VK_KHR_WIN32_SURFACE_EXTENSION_NAME};

	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName			= "Another Fucking Game";
	app_info.applicationVersion			= VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName				= "Dusty";
	app_info.engineVersion				= VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion					= VK_API_VERSION_1_0;

	instance_info.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo			= &app_info;
	instance_info.enabledLayerCount			= 0;
	instance_info.enabledExtensionCount		= extensiosn.size();
	instance_info.ppEnabledExtensionNames	= extensiosn.data();

	check_vk_error(vkCreateInstance(&instance_info, nullptr, &_instance), "vkCreateInstance");
}

void	Renderer::_create_vk_device()
{
	// Query how many devices are present in the system
	uint32_t deviceCount = 0;
	check_vk_error(vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr), "vkEnumeratePhysicalDevices");

	if (deviceCount == 0)
	{
	    std::cerr << "error! There is Vulkan physical device." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Get the physical devices
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	check_vk_error(vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data()), "vkEnumeratePhysicalDevices");


	VkPhysicalDeviceProperties deviceProperties;
	for (size_t i = 0; i < deviceCount; ++i)
	{
	    memset(&deviceProperties, 0, sizeof deviceProperties);
	    vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
	    std::cout << "Driver Version: " << deviceProperties.driverVersion << std::endl;
	    std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
	    std::cout << "Device Type: " << deviceProperties.deviceType << std::endl;
	    std::cout << "API Version: "
			<< ((deviceProperties.apiVersion>>22)&0x3FF) << " "
			<< ((deviceProperties.apiVersion>>12)&0x3FF) << " "
			<< (deviceProperties.apiVersion&0xFFF)
			<< std::endl;
	}

	VkDeviceCreateInfo deviceInfo = {};
	// Mandatory fields
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.flags = 0;

	// We won't bother with extensions or layers
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.ppEnabledExtensionNames = nullptr;

	// We don't want any any features,:the wording in the spec for DeviceCreateInfo
	// excludes that you can pass NULL to disable features, which GetPhysicalDeviceFeatures
	// tells you is a valid value. This must be supplied - NULL is legal here.
	deviceInfo.pEnabledFeatures = NULL;

	// Here's where we initialize our queues
	VkDeviceQueueCreateInfo deviceQueueInfo = {};
	deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueInfo.pNext = nullptr;
	deviceQueueInfo.flags = 0;
	// Use the first queue family in the family list
	deviceQueueInfo.queueFamilyIndex = 0;

	// Create only one queue
	float queuePriorities[] = { 1.0f };
	deviceQueueInfo.queueCount = 1;
	deviceQueueInfo.pQueuePriorities = queuePriorities;
	// Set queue(s) into the device
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &deviceQueueInfo;

	VkDevice _device;

	check_vk_error(vkCreateDevice(physicalDevices[0], &deviceInfo, nullptr, &_device), "vkCreateDevice");
}


void	Renderer::_create_vk_surface()
{
	SDL_SysWMinfo 	info;

	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo(_window, &info))
	{
		std::cerr << "error! SDL_GetWindowWMInfo()" << std::endl;
		exit(EXIT_FAILURE);
	}

	VkWin32SurfaceCreateInfoKHR surfacecreateInfo = {};
	surfacecreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfacecreateInfo.hwnd = info.info.win.window;
	surfacecreateInfo.hinstance = GetModuleHandle(nullptr);

	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = NULL;

	// somewhere in initialization code
	/**(void **)&vkGetPhysicalDeviceSurfaceFormatsKHR = vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	if (!vkPhysicalDeviceSurfaceFormatsKHR) {
	    abort();
	}*/

	check_vk_error(vkCreateWin32SurfaceKHR(_instance, &surfacecreateInfo, NULL, &_surface), "vkCreateWin32SurfaceKHR");
}

void	Renderer::set_fullscreen(bool const fullscreen)
{
	SDL_SetWindowFullscreen(_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void	Renderer::set_resolution(unsigned int const w, unsigned int const h)
{
	width = w;
	height = h;
}

void		Renderer::_set_buffer(Graphicengine const *ge)
{

}

void	Renderer::render(Graphicengine const *ge)
{
	Computedcamera	cm;

	_compute_camera(ge->camera, cm);

}
