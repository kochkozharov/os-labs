#ifndef LAB1_UTILS_H
#define LAB1_UTILS_H

#include <stddef.h>

enum PipeEnd {
    READ_END,
    WRITE_END
};

void ReverseString(char* string, size_t length);

#endif //LAB1_UTILS_H