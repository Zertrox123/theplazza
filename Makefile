##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## project makefile
##

SRC = ./src/Cook.cpp	\
	./src/main.cpp	\
	./src/ipc.cpp	\
	./src/kitchen.cpp	\
	./src/utils.cpp	\
	./src/plazza.cpp	\
	./src/Order.cpp

OBJ = $(SRC:.c=.o)
OBJ := $(OBJ:.cpp=.o)

CFLAGS   = -Wall -Wextra -Werror -pedantic -march=native -I./include/
CXXFLAGS = -Wall -Wextra -Werror -pedantic -march=native -std=c++20 -I./include/ -g3
LDFLAGS  = -pthread
NAME = plazza

CC = c++


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean re
