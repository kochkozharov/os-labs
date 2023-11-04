#ifndef BLUR_H
#define BLUR_H

#include <pthread.h>
#include <stddef.h>

#define MAX_CHANNELS (4)
#define DEF_THREAD_NUM (12)

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
    uc (*buf)[];
    size_t begin;
    size_t end;
    int times;
} ThreadArgs;

const uc* ApplyKernel(Image* img, const Kernel* kernel, int k, uc (*buffer)[], unsigned long threadsNum);

#endif  // BLUR_H