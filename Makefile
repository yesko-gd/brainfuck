CC=gcc
CFLAGS=-std=c23
BIN=bin/brainfuck
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRC))

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) -c $(CFLAGS) $^ -o $@
