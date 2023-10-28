#include <blur.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

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
            {1,1,1},
            {1,1,1},
            {1,1,1},
        },
    .order = 3,
    .divCoef = 9,
};


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "blur: missing 2 file operands\n");
        exit(EXIT_FAILURE);
    }

    int width;
    int height;
    int channels;
    stbi_uc *img = stbi_load(argv[1], &width, &height, &channels, 0);

    if (img == NULL) {
        perror("stbi_load");
        exit(EXIT_FAILURE);
    }

    printf("Loaded. x %dpx y %dpx channels %d.\n", width, height, channels);
    size_t imgSize = (size_t)width * height * channels;

    stbi_uc *newImg = malloc(imgSize);
    if (!newImg) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    ApplyKernel(&(Image){.matrix = (stbi_uc (*)[])img,
                         .width = width,
                         .height = height,
                         .channels = channels},
                &GAUSSIAN5, 5, (stbi_uc (*)[])newImg);
    stbi_write_jpg(argv[2], width, height, channels, newImg, 100);

    stbi_image_free(img);
    free(newImg);
    return 0;
}