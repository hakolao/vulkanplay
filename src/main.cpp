#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "vulkanplay.h"

int main(void) {
	VulkanPlayApp app;

	try {
		app.run(1280, 720, "Vulkan Play");
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
