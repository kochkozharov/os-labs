#ifndef BLUR_H
#define BLUR_H

#include <pthread.h>
#include <stddef.h>

#define MAX_CHANNELS (4)
#define THREAD_NUM (12)

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

typedef struct {
    Image* img;
    const Kernel* ker;
    pthread_barrier_t* barrier;
    uc (*out)[];
    size_t begin;
    size_t end;
    int times;
} ThreadArgs;

uc* ApplyKernel(Image* img, const Kernel* kernel, int k, uc (*output)[]);

#endif  // BLUR_H