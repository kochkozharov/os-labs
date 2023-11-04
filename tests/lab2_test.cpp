#include <gtest/gtest.h>

extern "C" {
#include "blur.h"
}

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <iostream>

namespace fs = std::filesystem;

TEST(SecondLabTests, SimpleTest) {
    const size_t width = 4;
    const size_t height = 2;
    uc imgMat[height][width] = {{1, 2, 3, 4}, {1, 2, 3, 4}};
    Image img = {reinterpret_cast<uc(*)[]>(imgMat), width, height, 1};
    const int kerMat[3][3] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    const Kernel ker = {reinterpret_cast<const int(*)[]>(kerMat), 3, 9};
    uc buf[height][width];
    uc excpectedRes[height][width] = {{1,2,3,3},{1,2,3,3}};
    uc *weakPtr =
        ApplyKernel(&img, &ker, 1, reinterpret_cast<uc(*)[]>(buf), 12);
    for (unsigned int i = 0; i < height; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            uc c = *(weakPtr+i*width+j);
            uc rc = excpectedRes[i][j];
            EXPECT_EQ(c, rc);
        }
    }
}