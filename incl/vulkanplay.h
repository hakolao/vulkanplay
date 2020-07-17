#ifndef VULKANPLAY_H
#define VULKANPLAY_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

#include "libcpp_matrix.h"

#define WIDTH 1280
#define HEIGHT 720

const int MAX_FRAMES_IN_FLIGHT = 2;
const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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
	optional<uint32_t> presentFamily;
	bool isComplete();
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	vector<VkSurfaceFormatKHR> formats;
	vector<VkPresentModeKHR> presentModes;
};

struct WindowData {
	uint32_t windowId;
	bool isHidden = false;
	void *parent;
};

class VulkanPlayApp {
   public:
	bool framebufferResized = false;
	WindowData windowData;

	void run(uint32_t width, uint32_t height, const char *name);
	static VKAPI_ATTR VkBool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
				  void *pUserData);
	static std::vector<char> readFile(const std::string &filename);

   private:
	// Todo Add width and height that change on resize
	bool isRunning = true;
	SDL_Window *window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	vector<VkCommandBuffer> commandBuffers;
	vector<VkSemaphore> imageAvailableSemaphores;
	vector<VkSemaphore> renderFinishedSemaphores;
	vector<VkFence> inFlightFences;
	vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	void createVulkanInstance(const char *name);
	void setupDebugMessenger();
	void createVulkanSurface();
	bool validVulkanExtensions(vector<const char *> extensionNames);
	bool checkValidationLayerSupport();
	int rateDeviceSuitability(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
	VkShaderModule createShaderModule(const std::vector<char> &code);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void recreateSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();
	void initWindow(uint32_t width, uint32_t height, const char *name);
	void initVulkan(const char *name);
	void mainLoop();
	void drawFrame();
	void cleanup();
	void cleanupSwapChain();
};

#endif
