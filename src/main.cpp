#include <iostream>
#include <stdexcept>

#include "vulkanplay.hpp"

int main(void) {
	VulkanPlayApp app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
