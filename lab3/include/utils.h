#ifndef LAB1_UTILS_H
#define LAB1_UTILS_H

#include <stddef.h>

enum PipeEnd {
    READ_END,
    WRITE_END
};

#define MAP_SIZE (1024U)
#define SHARED_MEMORY_NAME_1 "shared_memory_1"
#define SHARED_MEMORY_NAME_2 "shared_memory_2"
#define W_SEMAPHORE_1 "parent_semaphore_1"
#define W_SEMAPHORE_2 "parent_semaphore_2"
#define REV_SEMAPHORE_1 "rev_semaphore_1"
#define REV_SEMAPHORE_2 "rev_semaphore_2"

void ReverseString(char* string, size_t length);

#endif //LAB1_UTILS_H