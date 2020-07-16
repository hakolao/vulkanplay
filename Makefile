CC = g++
NAME=trystuff
DIR_SRC = ./src
DIR_OBJ = temp
LIBCPP_MATRIX = ./lib/libcpp_matrix
LIBVULKAN = ./lib/vulkan
LIBSDL2 = ./lib/SDL2
LIBCPP_MATRIX_FLAGS = -L$(LIBCPP_MATRIX) -lcpp_matrix
VULKAN_FLAGS =  -rpath $(LIBVULKAN) \
				-framework vulkan -F$(LIBVULKAN)/ \
				-rpath $(LIBSDL2) \
				-framework SDL2 -F$(LIBSDL2)/ \
				-framework SDL2_image -F$(LIBSDL2)/ \
				-framework SDL2_ttf -F$(LIBSDL2)/

INCL = -I ./incl -I $(LIBCPP_MATRIX)/incl \
		-I$(LIBVULKAN)/vulkan.framework/Headers \
		-I$(LIBSDL2)/SDL2.framework/Headers \
		-I$(LIBSDL2)/SDL2_image.framework/Headers \
		-I$(LIBSDL2)/SDL2_ttf.framework/Headers

FLAGS = -Wall -Wextra -Werror -O2 -std=c++17
SOURCES = main.cpp \
			vulkanplay.cpp

SRCS = $(addprefix $(DIR_SRC)/,$(SOURCES))
OBJS = $(addprefix $(DIR_OBJ)/,$(SOURCES:.cpp=.o))

all: $(DIR_OBJ) $(NAME)

$(NAME): $(OBJS)
	@make -C $(LIBCPP_MATRIX)
	$(CC) $(FLAGS) $(LIBCPP_MATRIX_FLAGS) $(VULKAN_FLAGS) -o $@ $^

$(DIR_OBJ):
	@mkdir -p temp

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp
	@$(CC) $(FLAGS) $(INCL) -c -o $@ $<

clean:
	@/bin/rm -f $(OBJS)
	@/bin/rm -rf $(DIR_OBJ)
	@make -C $(LIBCPP_MATRIX) clean

fclean: clean
	@/bin/rm -f $(NAME)
	@make -C $(LIBCPP_MATRIX) fclean

re: fclean all

.PHONY: all, $(DIR_OBJ), clean, fclean
