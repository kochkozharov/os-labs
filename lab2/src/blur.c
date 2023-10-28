#include "blur.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Convolution(const Image* img, size_t idx, const Kernel* ker,
                        uc (*out)[]) {
    uc(*imgMat)[img->width] = img->matrix;
    const int(*kerMat)[ker->order] = ker->matrix;
    uc(*outMat)[img->width] = out;
    const size_t absI = (idx / img->width) * img->channels;
    const size_t absJ = (idx % img->width) * img->channels;
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

void ApplyKernel(Image* img, const Kernel* kernel, int k, uc (*output)[]) {
    assert(kernel->order % 2 == 1 && img->channels <= MAX_CHANNELS);
    size_t matrixSize = img->height * img->width;
    for (int iteration = 0; iteration < k; ++iteration) {
        for (size_t i = 0; i < matrixSize; ++i) {
            Convolution(img, i, kernel, output);
        }
        uc(*temp)[] = img->matrix;
        img->matrix = output;
        output = temp;
    }
}