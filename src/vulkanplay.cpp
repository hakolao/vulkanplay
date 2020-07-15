#include "vulkanplay.h"

#include <iostream>
#include <vector>

using namespace std;

void VulkanPlayApp::run(uint32_t width, uint32_t height, const char *name) {
	ERROR_CHECK(SDL_Init(SDL_INIT_VIDEO) != 0, SDL_GetError());
	this->initWindow(width, height, name);
	this->initVulkan();
	this->mainLoop();
	this->cleanup();
}

void VulkanPlayApp::initWindow(uint32_t width, uint32_t height,
							   const char *name) {
	SDL_Window *window = SDL_CreateWindow(
		name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	ERROR_CHECK(window == NULL, SDL_GetError());
	this->window = window;
}

void VulkanPlayApp::initVulkan() {
	uint32_t extensionCount;
	ERROR_CHECK(
		!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr),
		"Failed to get instance extensions");
	vector<const char *> extensionNames(extensionCount);
	ERROR_CHECK(!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
												  extensionNames.data()),
				"Failed to get extension names");

	const VkApplicationInfo appInfo = {};

	vector<const char *> layerNames{};
	layerNames.push_back("VK_LAYER_KHRONOS_validation");

	VkInstanceCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pApplicationInfo = &appInfo;
	info.enabledLayerCount = layerNames.size();
	info.ppEnabledLayerNames = layerNames.data();
	info.enabledExtensionCount = extensionNames.size();
	info.ppEnabledExtensionNames = extensionNames.data();

	VkInstance instance;
	ERROR_CHECK(vkCreateInstance(&info, nullptr, &instance) != VK_SUCCESS,
				"Failed to create a Vulkan Instance");
	this->instance = instance;

	VkSurfaceKHR surface;
	ERROR_CHECK(!SDL_Vulkan_CreateSurface(window, instance, &surface),
				"Failed to create a Vulkan surface");
	this->surface = surface;
}

void VulkanPlayApp::mainLoop() {
	while (this->isRunning) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) isRunning = false;
		}
		// draw_frame();
	}
}

void VulkanPlayApp::cleanup() {
	vkDestroySurfaceKHR(this->instance, this->surface, 0);
	SDL_DestroyWindow(this->window);
	vkDestroyInstance(this->instance, 0);
	SDL_Quit();
}
