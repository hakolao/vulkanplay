#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include "vulkanplay.h"
#include "libcpp_matrix.h"

using namespace std;

SDL_Window		*createVulkanWindow(int width, int height, const char *name) {
	SDL_Window *window = SDL_CreateWindow(name,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	ERROR_CHECK(window == NULL, SDL_GetError());
	return window;
}

VkInstance	createVulkanInstance(SDL_Window *window) {
	uint32_t extensionCount;
	ERROR_CHECK(!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr),
		"Failed to get instance extensions");
	vector<const char *> extensionNames(extensionCount);
	ERROR_CHECK(!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames.data()),
		"Failed to get extension names");

	const VkApplicationInfo appInfo = {};

	vector<const char *> layerNames {};
	layerNames.push_back("VK_LAYER_KHRONOS_validation");

	VkInstanceCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pApplicationInfo = &appInfo;
	info.enabledLayerCount = layerNames.size();
	info.ppEnabledLayerNames = layerNames.data();
	info.enabledExtensionCount = extensionNames.size();
	info.ppEnabledExtensionNames = extensionNames.data();

	VkInstance instance;
	ERROR_CHECK(vkCreateInstance(&info, nullptr, &instance) != VK_SUCCESS,
		"Failed to create a Vulkan Instance");
	return instance;
}

VkSurfaceKHR	createVulkanSurface(SDL_Window *window, VkInstance instance) {
	VkSurfaceKHR surface;
	ERROR_CHECK(!SDL_Vulkan_CreateSurface(window, instance, &surface),
		"Failed to create a Vulkan surface");
	return surface;

}



int main(void) {
	bool isRunning = true;
	ERROR_CHECK(SDL_Init(SDL_INIT_VIDEO) != 0, SDL_GetError());
	SDL_Window *window = createVulkanWindow(1280, 720, "Vulkan Play");
	VkInstance instance = createVulkanInstance(window);
	VkSurfaceKHR surface = createVulkanSurface(window, instance);

	while (isRunning)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				isRunning = false;
		}

		// draw_frame();
	}

	vkDestroySurfaceKHR(instance, surface, 0);
	SDL_DestroyWindow(window);
	vkDestroyInstance(instance, 0);
	SDL_Quit();

	return 0;
}
