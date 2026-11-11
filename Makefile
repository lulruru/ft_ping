NAME    = ft_ping
CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -Iinclude

SRC_DIR = src
OBJ_DIR = obj
SRCS    = main.c parsing.c
OBJS    = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
DEPS    = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re