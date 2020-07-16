#ifndef VULKANPLAY_H
#define VULKANPLAY_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

#include "libcpp_matrix.h"

const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define ERROR_CHECK(test, message)                     \
	do {                                               \
		if ((test)) throw std::runtime_error(message); \
	} while (0)

struct QueueFamilyIndices {
	optional<uint32_t> graphicsFamily;
	bool isComplete();
};

class VulkanPlayApp {
   public:
	void run(uint32_t width, uint32_t height, const char *name);
	static VKAPI_ATTR VkBool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
				  void *pUserData);

   private:
	bool isRunning = true;
	SDL_Window *window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	void initWindow(uint32_t width, uint32_t height, const char *name);
	void initVulkan(const char *name);
	void createVulkanInstance(const char *name);
	void setupDebugMessenger();
	void createVulkanSurface();
	bool validVulkanExtensions(vector<const char *> extensionNames);
	bool checkValidationLayerSupport();
	void pickPhysicalDevice();
	void mainLoop();
	void cleanup();
};

#endif
