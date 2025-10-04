#include "brainfuck.h"
#include <stdio.h>
#include <memory.h>
#include <limits.h>

#ifdef WIN32

#else // WIN32
#include <termios.h>
#endif // WIN32

typedef enum {
    BRACKET_OPEN = 1,
    BRACKET_CLOSED = -1
} Bracket;

size_t memory_length;
uint8_t *memory;
size_t pointer;

void move_pointer(int64_t offset) {
    if (offset > 0 && pointer > SIZE_MAX - offset) {
        fprintf(stderr, "Reached memory size limit of %zu", SIZE_MAX);
        return;
    }

    size_t new_pointer = pointer;

    if (offset < 0 && pointer < (size_t)-offset) {
        size_t new_memory_length = memory_length - offset - pointer;
        memory = realloc(memory, new_memory_length);
        if (!memory) {
            fprintf(stderr, "Failed to allocate %zu bytes of memory\n", new_memory_length);
            return;
        }
        memset(memory, 0, new_memory_length - memory_length);

        memory_length = new_memory_length;

        new_pointer = 0;
    }
    else {
        new_pointer = pointer + offset;
    }

    if (new_pointer >= memory_length) {
        size_t new_memory_length = new_pointer + 1;
        memory = realloc(memory, new_memory_length);
        if (!memory) {
            fprintf(stderr, "Failed to allocate %zu bytes of memory\n", new_memory_length);
            return;
        }
        memset(memory + memory_length, 0, new_memory_length - memory_length);

        memory_length = new_memory_length;
    }

    pointer = new_pointer;
}

void set_cell(int16_t offset) {
    memory[pointer] += offset;
}

size_t find_matching_bracket(const size_t src_len, const char *src, size_t i, Bracket type) {
    uint32_t nest = 1;

    while (nest) {
        if (!i || i >= src_len) {
            fprintf(stderr, "%s bracket at %zu has no counterpart\n", type == BRACKET_CLOSED ? "Closing" : "Opening", pointer);
            return 0;
        }

        i += type;

        char ch = src[i];

        bool opening;

        if (ch == '[') {
            opening = true;
        }
        else if (ch == ']') {
            opening = false;
        }
        else {
            continue;
        }

        if (opening == (type == BRACKET_OPEN)) {
            nest++;
        }
        else {
            nest--;
        }
    }

    return i;
}

bool parse_ch(const size_t src_len, const char *src, size_t *i) {
    const char ch = src[*i];

    if (!ch) {
        return true;
    }

    switch (ch) {
        case '>':
            move_pointer(1);
            break;
        case '<':
            move_pointer(-1);
            break;
        case '+':
            set_cell(1);
            break;
        case '-':
            set_cell(-1);
            break;
        case '.':
            fputc(memory[pointer], stdout);
            break;
        case ',':
            memory[pointer] = fgetc(stdin);
            break;
        case '[':
            if (!memory[pointer]) {
                *i = find_matching_bracket(src_len, src, *i, BRACKET_OPEN);
            }
            break;
        case ']':
            if (memory[pointer]) {
                *i = find_matching_bracket(src_len, src, *i, BRACKET_CLOSED);
            }
            break;
    }

    return false;
}

int interpret(const size_t len, const char *src) {
    memory_length = BASE_MEMORY_SIZE;

    memory = calloc(memory_length, sizeof(uint8_t));
    if (!memory) {
        fprintf(stderr, "Failed to allocate %zu bytes of memory\n", memory_length);
        return 1;
    }

    pointer = memory_length / 2;

#ifdef WIN32

#else
    struct termios t;
    tcgetattr(0, &t);
    t.c_lflag &= ~ECHO;
    t.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &t);
#endif

    for (size_t i = 0; i < len; i++) {
        bool should_exit = parse_ch(len, src, &i);
        if (should_exit) {
            break;
        }
    }

    printf("\n");

#ifdef WIN32

#else
    tcgetattr(0, &t);
    t.c_lflag |= ECHO;
    t.c_lflag |= ICANON;
    tcsetattr(0, TCSANOW, &t);
#endif

    return 0;
}
