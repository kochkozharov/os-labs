#include <gtest/gtest.h>

#include <cstring>
#include <utility>

#include "lib.h"

TEST(FourthLabTests, GCDTest) {
    EXPECT_EQ(GCD(49, 56), 7);
    EXPECT_EQ(GCD(56, 49), 7);
    EXPECT_EQ(GCD(57, 49), 1);
}

TEST(FourthLabTests, TranslationTest) {
    auto *str = Translation(0);
    EXPECT_TRUE(std::strcmp(str, "0") == 0);
    std::free(str);
    str = Translation(5);
    EXPECT_TRUE(std::strcmp(str, "12") == 0);
    std::free(str);
    str = Translation(888);
    EXPECT_TRUE(std::strcmp(str, "1012220") == 0);
    str = Translation(-1);
    EXPECT_EQ(str, nullptr);
    std::free(str);
}