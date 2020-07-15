#ifndef VULKANPLAY_H
# define VULKANPLAY_H

# include <stdio.h>
# include <vulkan/vulkan.h>
# include <SDL.h>
# include <SDL_vulkan.h>
# include "libcpp_matrix.h"

# define ERROR_CHECK(test, message) \
	do { \
		if((test)) { \
			dprintf(2, "%s\n", (message)); \
			exit(1); \
		} \
	} while(0)

class VulkanPlayApp {
	public:
		void run(uint32_t width, uint32_t height, const char *name);
	private:
		bool			isRunning = true;
		SDL_Window		*window;
		VkInstance		instance;
		VkSurfaceKHR	surface;
		void	initWindow(uint32_t width, uint32_t height, const char *name);
		void	initVulkan();
		void	mainLoop();
		void	cleanup();
};

#endif
