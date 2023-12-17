#include "utils.h"
#include <cstddef>

void ReverseString(char* string, std::size_t length) {
    for (std::size_t i = 0; i < length >> 1; ++i) {
        char temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}