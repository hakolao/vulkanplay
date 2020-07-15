#ifndef VULKANPLAY_H
# define VULKANPLAY_H

#include <stdio.h>
#include <iostream>
#include <vector>

# define ERROR_CHECK(test, message) \
	do { \
		if((test)) { \
			dprintf(2, "%s\n", (message)); \
			exit(1); \
		} \
	} while(0)

#endif
