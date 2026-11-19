NAME    = ft_ping
CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -Iinclude

SRC_DIR = src
OBJ_DIR = obj
SRCS    = checksum.c main.c packet.c parsing.c ping.c socket.c stats.c
OBJS    = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
DEPS    = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lm

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