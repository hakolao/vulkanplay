#include "vulkanplay.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

bool QueueFamilyIndices::isComplete() {
	return graphicsFamily.has_value() && presentFamily.has_value();
}

static std::vector<const char *> getRequiredExtensions(SDL_Window *window) {
	uint32_t extensionCount = 0;
	ERROR_CHECK(
		!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr),
		"Failed to get instance extensions");
	vector<const char *> extensionNames(extensionCount);
	ERROR_CHECK(!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
												  extensionNames.data()),
				"Failed to get extension names");

	if (enableValidationLayers) {
		extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensionNames;
}

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
	const VkAllocationCallbacks *pAllocator,
	VkDebugUtilsMessengerEXT *pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugUtilsMessengerEXT(
	VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks *pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

static void populateDebugMessengerCreateInfo(
	VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanPlayApp::debugCallback;
}

int VulkanPlayApp::rateDeviceSuitability(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;
	// if (!deviceFeatures.geometryShader) {
	// 	return 0;
	// }

	QueueFamilyIndices indices = findQueueFamilies(device);
	if (!indices.isComplete()) return 0;

	return score;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanPlayApp::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData) {
	(void)messageType;
	(void)pUserData;
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "validation layer: " << pCallbackData->pMessage
				  << std::endl;
	}
	return VK_FALSE;
}

void VulkanPlayApp::run(uint32_t width, uint32_t height, const char *name) {
	ERROR_CHECK(SDL_Init(SDL_INIT_VIDEO) != 0, SDL_GetError());
	initWindow(width, height, name);
	initVulkan(name);
	setupDebugMessenger();
	pickPhysicalDevice();
	createLogicalDevice();
	mainLoop();
	cleanup();
}

QueueFamilyIndices VulkanPlayApp::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
											 nullptr);

	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
											 queueFamilies.data());

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (indices.isComplete()) {
			break;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
											 &presentSupport);
		if (presentSupport) {
			indices.presentFamily = i;
		}
		i++;
	}
	return indices;
}

void VulkanPlayApp::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
										 indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount =
		static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount =
			static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	ERROR_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
					VK_SUCCESS,
				"Failed to create logical device!");
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void VulkanPlayApp::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	ERROR_CHECK(deviceCount == 0, "Failed to find GPUs with Vulkan support!");
	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto &device : devices) {
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}
	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first > 0)
		physicalDevice = candidates.rbegin()->second;
	else
		ERROR_CHECK(true, "Failed to find a suitable GPU!");
}

void VulkanPlayApp::setupDebugMessenger() {
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);
	ERROR_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
											 &debugMessenger) != VK_SUCCESS,
				"failed to set up debug messenger!");
}

void VulkanPlayApp::initVulkan(const char *name) {
	createVulkanInstance(name);
	createVulkanSurface();
}

void VulkanPlayApp::initWindow(uint32_t width, uint32_t height,
							   const char *name) {
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, width, height,
							  SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	ERROR_CHECK(window == NULL, SDL_GetError());
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

	for (const auto &layerName : validationLayers) {
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
	return true;
}

void VulkanPlayApp::createVulkanInstance(const char *name) {
	ERROR_CHECK(enableValidationLayers && !checkValidationLayerSupport(),
				"validation layers requested, but not available!");
	auto extensionNames = getRequiredExtensions(window);
	ERROR_CHECK(!validVulkanExtensions(extensionNames),
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
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount =
			static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext =
			(VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}
	createInfo.enabledExtensionCount = extensionNames.size();
	createInfo.ppEnabledExtensionNames = extensionNames.data();

	ERROR_CHECK(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS,
				"Failed to create a Vulkan Instance");
}

void VulkanPlayApp::createVulkanSurface() {
	ERROR_CHECK(!SDL_Vulkan_CreateSurface(window, instance, &surface),
				"Failed to create a Vulkan surface");
}

void VulkanPlayApp::mainLoop() {
	while (isRunning) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) isRunning = false;
		}
		// draw_frame();
	}
}

void VulkanPlayApp::cleanup() {
	vkDestroyDevice(device, nullptr);
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(instance, surface, 0);
	SDL_DestroyWindow(window);
	vkDestroyInstance(instance, 0);
	SDL_Quit();
}
