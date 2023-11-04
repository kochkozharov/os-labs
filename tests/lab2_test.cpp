#include <gtest/gtest.h>

extern "C" {
#include "blur.h"
}

TEST(SecondLabTests, SimpleTest) {
    const size_t width = 4;
    const size_t height = 2;
    const uc example[height][width] = {{1, 2, 3, 4}, {1, 2, 3, 4}};
    uc imgMat[height][width];
    memcpy(imgMat, example, width * height);
    Image img = {reinterpret_cast<uc(*)[]>(imgMat), width, height, 1};
    const int kerMat[3][3] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    const Kernel ker = {reinterpret_cast<const int(*)[]>(kerMat), 3, 9};
    uc buf[height][width];
    const uc excpectedRes[height][width] = {{1, 2, 3, 3}, {1, 2, 3, 3}};
    uc *weakPtr = ApplyKernel(&img, &ker, 1, reinterpret_cast<uc(*)[]>(buf), 2);
    for (unsigned int i = 0; i < height; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            uc c = *(weakPtr + i * width + j);
            uc rc = excpectedRes[i][j];
            EXPECT_EQ(c, rc);
        }
    }
    //тот же результат при многопоточной обработке
    memcpy(imgMat, example, width * height);
    img.matrix = reinterpret_cast<uc(*)[]>(imgMat);
    weakPtr = ApplyKernel(&img, &ker, 1, reinterpret_cast<uc(*)[]>(buf), 1);
    for (unsigned int i = 0; i < height; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            uc c = *(weakPtr + i * width + j);
            uc rc = excpectedRes[i][j];
            EXPECT_EQ(c, rc);
        }
    }
}