CC= clang
CFLAGS= -Wall -Werror -Wextra
LDFLAGS=
TARGET=lalala

SRC:= src/main.c src/lalala.c src/lll_sprintf.c
OBJ:= $(SRC:.c=.o)

ifdef DEBUG
CFLAGS += -DDEBUG -g
LDFLAGS += -DDEBUG -g
else ifdef ASAN
CFLAGS += -DDEBUG -g -fsanitize=address
LFFLAGS += -DDEBUG -g -fsanitize=address
else
CFLAGS += -O3
LDFLAGS += -O3
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
