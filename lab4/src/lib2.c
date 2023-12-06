#include <stdlib.h>
#include <string.h>

#include "lib.h"

void ReverseString(char* string, size_t length);

char* Translation(long x) {
    if (x < 0) {
        return NULL;
    }
    char* trinary = (char*)malloc(NUM_BUFER_SIZE);
    if (!trinary) {
        return trinary;
    }
    int i = 0;
    do {
        trinary[i++] = '0' + (x % 3);
        x /= 3;
    } while (x);
    trinary[i] = '\0';
    ReverseString(trinary, i);
    return trinary;
}

int GCD(int a, int b) {
    if (a < 0 || b < 0) {
        return -1;
    }
    int min = a < b ? a : b;
    int gcd = 0;
    for (int i = 1; i < min + 1; i++) {
        if ((a % i) == 0 && (b % i) == 0) {
            gcd = i;
        }
    }
    return gcd;
}

void ReverseString(char* string, size_t length) {
    for (size_t i = 0; i < length >> 1; ++i) {
        char temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}
