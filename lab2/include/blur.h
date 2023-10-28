#ifndef BLUR_H
#define BLUR_H

#include <stddef.h>

#define MAX_CHANNELS (4)

typedef unsigned char uc;

typedef struct {
    uc (*matrix)[];
    size_t width;
    size_t height;
    int channels;
} Image;

typedef struct {
    const int (*matrix)[];
    int order;
    int divCoef;
} Kernel;

void ApplyKernel(Image* img, const Kernel* kernel, int k, uc (*output)[]);

#endif  // BLUR_H