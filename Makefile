CC= clang
CFLAGS= -Wall -Werror -Wextra -g
LDFLAGS= -g
TARGET=lalala

SRC:= src/main.c src/lll_sprintf.c
OBJ:= $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)
