CC = g++
NAME=trystuff
DIR_SRC = ./src
DIR_OBJ = temp
LIBCPP_MATRIX = ./lib/libcpp_matrix
LIBVULKAN = ./lib/vulkan
LIBCPP_MATRIX_FLAGS = -L$(LIBCPP_MATRIX) -lcpp_matrix
VULKAN_FLAGS =  -Wl,-rpath -Wl,$(LIBVULKAN) \
				-framework vulkan -F$(LIBVULKAN)/
INCL = -I ./incl -I $(LIBCPP_MATRIX)/incl \
		-I$(LIBVULKAN)/vulkan.framework/Headers
FLAGS = -Wall -Wextra -Werror -O2 -std=c++11
SOURCES = main.cpp

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
