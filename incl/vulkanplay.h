#ifndef VULKANPLAY_H
#define VULKANPLAY_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#include <vector>

#include "libcpp_matrix.h"

#define NDEBUG 1

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
	bool validVulkanExtensions(vector<const char *> extensionNames);
	bool checkValidationLayerSupport();

	std::vector<const char *> validationLayers;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
};

#endif
