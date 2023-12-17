#include <gtest/gtest.h>

#include <cstring>

#include "lib.h"

TEST(FourthLabTests, GCDTest) {
    EXPECT_EQ(GCD(49, 56), 7);
    EXPECT_EQ(GCD(56, 49), 7);
    EXPECT_EQ(GCD(57, 49), 1);
}

TEST(FourthLabTests, TranslationTest) {
    auto deleter = [](char *str) { std::free(str); };
    std::unique_ptr<char, decltype(deleter)> str(Translation(31), deleter);
    EXPECT_TRUE(std::strcmp(str.get(), "11111") == 0);
    str.reset(Translation(0));
    EXPECT_TRUE(std::strcmp(str.get(), "0") == 0);
    str.reset(Translation(888));
    EXPECT_TRUE(std::strcmp(str.get(), "1101111000") == 0);
    str.reset(Translation(-1));
    EXPECT_EQ(str.get(), nullptr);
}