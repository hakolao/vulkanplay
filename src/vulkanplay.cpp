#include "vulkanplay.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

void VulkanPlayApp::run(uint32_t width, uint32_t height, const char *name) {
	ERROR_CHECK(SDL_Init(SDL_INIT_VIDEO) != 0, SDL_GetError());
	this->initWindow(width, height, name);
	this->initVulkan(name);
	this->mainLoop();
	this->cleanup();
}

void VulkanPlayApp::initVulkan(const char *name) {
	this->validationLayers.push_back("VK_LAYER_KHRONOS_validation");
	this->createVulkanInstance(name);
	this->createVulkanSurface();
}

void VulkanPlayApp::initWindow(uint32_t width, uint32_t height,
							   const char *name) {
	SDL_Window *window = SDL_CreateWindow(
		name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	ERROR_CHECK(window == NULL, SDL_GetError());
	this->window = window;
}

bool VulkanPlayApp::validVulkanExtensions(vector<const char *> extensionNames) {
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
										   availableExtensions.data());
	uint32_t matchCount = 0;
	for (const auto &extension : availableExtensions)
		for (const auto &name : extensionNames)
			if (strcmp(name, extension.extensionName) == 0) matchCount++;
	return matchCount == extensionNames.size();
}

bool VulkanPlayApp::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : this->validationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}
	return false;
}

void VulkanPlayApp::createVulkanInstance(const char *name) {
	ERROR_CHECK(enableValidationLayers && !checkValidationLayerSupport(),
				"validation layers requested, but not available!");
	uint32_t extensionCount = 0;
	ERROR_CHECK(
		!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr),
		"Failed to get instance extensions");
	vector<const char *> extensionNames(extensionCount);
	ERROR_CHECK(!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
												  extensionNames.data()),
				"Failed to get extension names");

	ERROR_CHECK(!this->validVulkanExtensions(extensionNames),
				"Some SDL vulkan extensions not found in availalbe extensions");

	VkApplicationInfo appInfo = {};
	appInfo.pApplicationName = name;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount =
			static_cast<uint32_t>(this->validationLayers.size());
		createInfo.ppEnabledLayerNames = this->validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	createInfo.enabledExtensionCount = extensionNames.size();
	createInfo.ppEnabledExtensionNames = extensionNames.data();

	VkInstance instance;
	ERROR_CHECK(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS,
				"Failed to create a Vulkan Instance");
	this->instance = instance;
}

void VulkanPlayApp::createVulkanSurface() {
	VkSurfaceKHR surface;
	ERROR_CHECK(
		!SDL_Vulkan_CreateSurface(this->window, this->instance, &surface),
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
