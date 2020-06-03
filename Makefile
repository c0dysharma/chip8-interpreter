CC = gcc
SRC = src/*.c
CFLAGS = -g -O0 -std=c11 -Werror
LFLAGS = `sdl2-config --cflags --libs` 

all:
	$(CC) $(SRC) $(CFLAGS) $(LFLAGS) -o ./bin/chip8
