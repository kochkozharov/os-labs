#include "blur.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void Convolution(const Image* img, size_t idx, const Kernel* ker,
                        uc (*out)[]) {
    uc(*imgMat)[img->width] = img->matrix;
    const int(*kerMat)[ker->order] = ker->matrix;
    uc(*outMat)[img->width] = out;
    const size_t absI = idx / img->width * img->channels;
    const size_t absJ = idx % img->width * img->channels;
    size_t sum[MAX_CHANNELS] = {0};

    for (int i = -(ker->order / 2); i <= ker->order / 2; ++i) {
        for (int j = -ker->order / 2; j <= ker->order / 2; ++j) {
            const int kerI = i + ker->order / 2;
            const int kerJ = j + ker->order / 2;

            size_t imgI;
            if (i < 0 && (size_t)-i * img->channels > absI) {
                imgI = 0;
            } else {
                imgI = (size_t)i * img->channels + absI;
                if (imgI > (img->height - 1) * img->channels) {
                    imgI = (img->height - 1) * img->channels;
                }
            }

            size_t imgJ;
            if (j < 0 && (size_t)-j * img->channels > absJ) {
                imgJ = 0;
            } else {
                imgJ = (size_t)j * img->channels + absJ;
                if (imgJ > (img->width - 1) * img->channels) {
                    imgJ = (img->width - 1) * img->channels;
                }
            }

            int coef = kerMat[kerI][kerJ];
            for (int k = 0; k < img->channels; ++k) {
                sum[k] += (size_t)coef * imgMat[imgI][imgJ + k];
            }
        }
    }
    for (int k = 0; k < img->channels; ++k) {
        outMat[absI][absJ + k] = sum[k] / ker->divCoef;
    }
}
static void* ChunkConvolution(void* ptr) {
    ThreadArgs* arg = ptr;
    uc(*buf)[] = arg->buf;
    for (int iteration = 0; iteration < arg->times; ++iteration) {
        for (size_t i = arg->begin; i < arg->end; ++i) {
            Convolution(arg->img, i, arg->ker, buf);
        }
        int status = pthread_barrier_wait(arg->barrier);
        if (status != 0 && status != PTHREAD_BARRIER_SERIAL_THREAD) {
            perror("pthread_barrier_wait");
            exit(EXIT_FAILURE);
        }
        uc(*temp)[] = arg->img->matrix;
        arg->img->matrix = buf;
        buf = temp;
    }
    return NULL;
}

uc* ApplyKernel(Image* img, const Kernel* kernel, int k, uc (*buffer)[], unsigned long threadsNum) {
    assert(kernel->order % 2 == 1 && img->channels <= MAX_CHANNELS);
    int status;
    size_t matrixSize = img->height * img->width;
    pthread_barrier_t barrier;
    status = pthread_barrier_init(&barrier, NULL, threadsNum);
    if (status != 0) {
        perror("pthread_barrier_init");
        exit(status);
    }

    pthread_t *threads=malloc(threadsNum*sizeof(pthread_t));
    ThreadArgs *args=malloc(threadsNum*sizeof(ThreadArgs));
    size_t pixelsPerThread = matrixSize / threadsNum;

    for (unsigned long i = 0; i < threadsNum; ++i) {
        size_t begin = i * pixelsPerThread;
        size_t end = i == threadsNum - 1 ? matrixSize : begin + pixelsPerThread;
        args[i] = (ThreadArgs){.img = img,
                               .begin = begin,
                               .end = end,
                               .ker = kernel,
                               .buf = buffer,
                               .times = k,
                               .barrier = &barrier};
        status = pthread_create(&threads[i], NULL, ChunkConvolution, &args[i]);
        if (status != 0) {
            perror("pthread_create");
            exit(status);
        }
    }
    for (unsigned long i = 0; i < threadsNum; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    free(threads);
    free(args);
    return (uc*)(k % 2 == 1 ? buffer : img->matrix);
}