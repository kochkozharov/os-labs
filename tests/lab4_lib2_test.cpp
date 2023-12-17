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
    auto deleter = [](char *str) { std::free(str); };
    std::unique_ptr<char, decltype(deleter)> str(Translation(0), deleter);
    EXPECT_TRUE(std::strcmp(str.get(), "0") == 0);
    str.reset(Translation(5));
    EXPECT_TRUE(std::strcmp(str.get(), "12") == 0);
    str.reset(Translation(888));
    EXPECT_TRUE(std::strcmp(str.get(), "1012220") == 0);
    str.reset(Translation(-1));
    EXPECT_EQ(str.get(), nullptr);
}