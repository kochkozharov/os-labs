#include <getopt.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <time.h>

#include "blur.h"

//#define TEST
//для теста эффективности многопоточности

typedef enum { gauss, box } TFilter;

static const Kernel GAUSSIAN5 = {
    .matrix =
        (const int[5][5]){
            {1, 4, 6, 4, 1},
            {4, 16, 24, 16, 4},
            {6, 24, 36, 24, 6},
            {4, 16, 24, 16, 4},
            {1, 4, 6, 4, 1},
        },
    .order = 5,
    .divCoef = 256,
};

static const Kernel BOX3 = {
    .matrix =
        (const int[3][3]){
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1},
        },
    .order = 3,
    .divCoef = 9,
};

int main(int argc, char *argv[]) {
#ifdef TEST
    (void)BOX3;
    if (argc < 2) {
        fprintf(stderr,
                "Usage: blur INPUT_FNAME\n");
        exit(EXIT_SUCCESS);
    }
#else
    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,
                "Usage: blur INPUT_FNAME OUTPUT_FNAME -f FILTER -k K (apply "
                "filter FILTER K times)\n");
        exit(EXIT_SUCCESS);
    }

    long times = 1;
    unsigned long threadsNum = DEF_THREAD_NUM;
    TFilter filter = gauss;
    const char *filterName;
    for (int opt; opt = getopt(argc, argv, "f:k:t:r"), opt != -1;) {
        switch (opt) {
            case '?':
                perror("getopt");
                exit(EXIT_FAILURE);
            case 'f':
                if (strcmp(optarg, "box") == 0) {
                    filter = box;
                }
                break;
            case 'k': {
                char *end;
                times = strtol(optarg, &end, 10);
                break;
            }
            case 't': {
                char *end;
                threadsNum = strtol(optarg, &end, 10);
                break;
            }
        }
    }
#endif

    int width;
    int height;
    int channels;

#ifndef TEST
    stbi_uc *img = stbi_load(argv[argc - 2], &width, &height, &channels, 0);
#else
    stbi_uc *img = stbi_load(argv[1], &width, &height, &channels, 0);
#endif

    if (img == NULL) {
        perror("stbi_load");
        exit(EXIT_FAILURE);
    }
    printf("Loaded. x: %dpx y: %dpx channels: %d.\n", width, height, channels);

    size_t imgSize = (size_t)width * height * channels;
    stbi_uc *buf = malloc(imgSize);
    if (!buf) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    const Kernel *ker = &GAUSSIAN5;

#ifdef TEST
    const stbi_uc *weakPtr;
    for (int threads = 1; threads < 21; ++threads) {
        printf("Applying gaussian blur 20 times on %d threads\n", threads);
#else
    if (filter == box) {
        ker = &BOX3;
        filterName = "box";
    } else {
        filterName = "gaussian";
    }
    printf("Applying %s blur %ld times on %ld threads\n", filterName, times,
           threadsNum);
#endif

        struct timespec start;
        struct timespec finish;
        clock_gettime(CLOCK_MONOTONIC, &start);
#ifdef TEST
        weakPtr = ApplyKernel(&(Image){.matrix = (stbi_uc(*)[])img,
                                       .width = width,
                                       .height = height,
                                       .channels = channels},
                              ker, 20, (stbi_uc(*)[])buf, threads);
#else
    const stbi_uc *weakPtr =
        ApplyKernel(&(Image){.matrix = (stbi_uc(*)[])img,
                             .width = width,
                             .height = height,
                             .channels = channels},
                    ker, (int)times, (stbi_uc(*)[])buf, threadsNum);
#endif
        clock_gettime(CLOCK_MONOTONIC, &finish);
        double elapsed;
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1.0E9;
        printf("Function took %fs to execute\n", elapsed);

#ifdef TEST
    }
    printf("Test end\n");
#endif

#ifdef TEST
    int status =
        stbi_write_jpg("result.jpg", width, height, channels, weakPtr, 100);
#else
    int status =
        stbi_write_jpg(argv[argc - 1], width, height, channels, weakPtr, 100);
#endif

    if (status == 1) {
        printf("Successfully written %zu bytes\n",
               (size_t)width * height * channels);
    } else {
        perror("stbi_write_jpg");
        exit(EXIT_FAILURE);
    }

    stbi_image_free(img);
    free(buf);
    return 0;
}