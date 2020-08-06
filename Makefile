CC = g++
NAME=trystuff
DIR_SRC = ./src
DIR_OBJ = temp
LIBVULKAN = ./lib/vulkan
LIBSDL2 = ./lib/SDL2
TINYOBJLOADER = ./lib/tinyobjloader
# USE THESE AT SCHOOL
BREW_INCLUDES = /Users/ohakola/.brew/include #for glm
VULKAN_SDK_PATH = /Users/ohakola/goinfre/vulkan/macOS
# ===


VULKAN_FLAGS =  -rpath $(LIBVULKAN) \
				-framework vulkan -F$(LIBVULKAN)/ \
				-rpath $(LIBSDL2) \
				-framework SDL2 -F$(LIBSDL2)/ \
				-framework SDL2_image -F$(LIBSDL2)/ \
				-framework SDL2_ttf -F$(LIBSDL2)/

INCL = -I$(BREW_INCLUDES) \
		-I$(VULKAN_SDK_PATH)/include \
		-I ./incl \
		-I$(TINYOBJLOADER) \
		-I$(LIBVULKAN)/vulkan.framework/Headers \
		-I$(LIBSDL2)/SDL2.framework/Headers \
		-I$(LIBSDL2)/SDL2_image.framework/Headers \
		-I$(LIBSDL2)/SDL2_ttf.framework/Headers

FLAGS = -Wall -Wextra -Werror -O3 -std=c++17
SOURCES = main.cpp \
			vulkanplay.cpp

SRCS = $(addprefix $(DIR_SRC)/,$(SOURCES))
OBJS = $(addprefix $(DIR_OBJ)/,$(SOURCES:.cpp=.o))

all: $(DIR_OBJ) $(NAME)

$(NAME): $(OBJS)
	@printf "\033[32;1mCompiling shaders...\n\033[0m"
	cd shaders && /bin/sh compile.sh && cd ..
	@printf "\033[32;1mCompiling app...\n\033[0m"
	$(CC) $(FLAGS) $(VULKAN_FLAGS) -o $@ $^
	@printf "\033[32;1mDone. Run: ./$(NAME)\n\033[0m"

$(DIR_OBJ):
	@mkdir -p temp

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp
	@$(CC) $(FLAGS) $(INCL) -c -o $@ $<

clean:
	@/bin/rm -f $(OBJS)
	@/bin/rm -rf $(DIR_OBJ)
	@/bin/rm -f shaders/*.spv

fclean: clean
	@/bin/rm -f $(NAME)

re: fclean all

.PHONY: all, $(DIR_OBJ), clean, fclean
