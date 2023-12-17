#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "utils.h"

char* Translation(long x) {
    if (x < 0) {
        return nullptr;
    }
    char* binary = (char*)malloc(NUM_BUFFER_SIZE);
    if (!binary) {
        return binary;
    }
    int i = 0;
    do {
        binary[i++] = '0' + (x & 1);
        x >>= 1;
    } while(x);
    binary[i] = '\0';
    ReverseString(binary, i);
    return binary;
}

int GCD(int a, int b) {
    if (a < 0 || b < 0) {
        return -1;
    }
    while (b != 0) {
        int tmp;
        tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}
