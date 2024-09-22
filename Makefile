CC= clang
CFLAGS= -Wall -Werror -Wextra -g
LDFLAGS= -g
TARGET=lalala

SRC:= src/main.c src/lll_sprintf.c
OBJ:= $(SRC:.c=.o)

ifdef DEBUG
CFLAGS += -DDEBUG
LDFLAGS += -DDEBUG
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re
