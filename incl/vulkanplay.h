#ifndef VULKANPLAY_H
#define VULKANPLAY_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#include <array>
// ToDo don't use glm...
#include <glm/glm.hpp>
#include <optional>
#include <vector>

using namespace std;

// ToDo use own libcpp matrix
// #include "libcpp_matrix.h"

#define WIDTH 1280
#define HEIGHT 720
#define NAME "Vulkan Play"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;
const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct VulkanVertex {
	glm::vec2 pos;
	glm::vec3 color;
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2>
	getAttributeDescriptions();
};

const std::vector<VulkanVertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

// const std::vector<Vertex> vertices = {
// 	Vertex(0.0f, -0.5f, 0.0f, Color(1.0f, 0.0f, 0.0f)),
// 	Vertex(0.5f, 0.5f, 0.0f, Color(0.0f, 1.0f, 0.0f)),
// 	Vertex(-0.5f, 0.5f, 0.0f, Color(0.0f, 0.0f, 1.0f))};

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

	void run();
	static VKAPI_ATTR VkBool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
				  void *pUserData);
	static std::vector<char> readFile(const std::string &filename);

   private:
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
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	void createVulkanInstance();
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
	uint32_t findMemoryType(uint32_t typeFilter,
							VkMemoryPropertyFlags properties);
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
	void createVertexBuffer();
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties, VkBuffer &buffer,
					  VkDeviceMemory &bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void initWindow();
	void initVulkan();
	void mainLoop();
	void drawFrame();
	void cleanup();
	void cleanupSwapChain();
};

#endif
