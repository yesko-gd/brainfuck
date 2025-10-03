#include "brainfuck.h"
#include <stdio.h>
#include <string.h>

#define PATH_LENGTH 1024

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: brainfuck [source-file]\n");
        return 1;
    }

    char src_path[PATH_LENGTH] = { 0 };
    strncpy(src_path, argv[1], PATH_LENGTH);

    FILE *src_file = fopen(src_path, "r");
    if (!src_file) {
        fprintf(stderr, "Failed to open '%s'", src_path);
        return 1;
    }

    fseek(src_file, 0, SEEK_END);
    size_t src_len = ftell(src_file);
    fseek(src_file, 0, SEEK_SET);

    char *src = malloc(src_len);
    if (!src) {
        fprintf(stderr, "Failed to allocate %zu bytes of memory for source code\n", src_len);
        return 1;
    }

    fread(src, 1, src_len, src_file);

    int err = interpret(src_len, src);

    free(src);

    return err;
}
