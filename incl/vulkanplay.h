#ifndef VULKANPLAY_H
#define VULKANPLAY_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#include "libcpp_matrix.h"

#define ERROR_CHECK(test, message)                     \
	do {                                               \
		if ((test)) throw std::runtime_error(message); \
	} while (0)

class VulkanPlayApp {
   public:
	void run(uint32_t width, uint32_t height, const char *name);

   private:
	bool isRunning = true;
	SDL_Window *window;
	VkInstance instance;
	VkSurfaceKHR surface;
	void initWindow(uint32_t width, uint32_t height, const char *name);
	void initVulkan(const char *name);
	void createVulkanInstance(const char *name);
	void createVulkanSurface();
	void mainLoop();
	void cleanup();
};

#endif
