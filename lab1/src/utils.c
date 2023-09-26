#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void ReverseString(char* string, size_t length) {
    for (size_t i = 0; i < length >> 1; ++i) {
        char temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}
