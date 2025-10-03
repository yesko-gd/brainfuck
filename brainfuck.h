#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include <stdlib.h>
#include <stdint.h>

#define BASE_MEMORY_SIZE 256

int interpret(size_t len, const char *src);

#endif // !BRAINFUCK_H
