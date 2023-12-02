#ifndef LAB1_UTILS_H
#define LAB1_UTILS_H

#include <stddef.h>

enum PipeEnd {
    READ_END,
    WRITE_END
};

#define MAP_SIZE (1024U)
#define SHARED_MEMORY_NAME_1 ".shared_memory_1"
#define SHARED_MEMORY_NAME_2 ".shared_memory_2"

void ReverseString(char* string, size_t length);

#endif //LAB1_UTILS_H