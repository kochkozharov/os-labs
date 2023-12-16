#include <gtest/gtest.h>

#include <cstring>
#include "lib.h"

TEST(FourthLabTests, GCDTest) {
    EXPECT_EQ(GCD(49, 56), 7);
    EXPECT_EQ(GCD(56, 49), 7);
    EXPECT_EQ(GCD(57, 49), 1);

}

TEST(FourthLabTests, TranslationTest) {
    char *str = Translation(31);
    EXPECT_TRUE(std::strcmp(str, "11111") == 0);
    std::free(str);
    str = Translation(0);
    EXPECT_TRUE(std::strcmp(str, "0") == 0);
    std::free(str);
    str = Translation(888);
    EXPECT_TRUE(std::strcmp(str, "1101111000") == 0);
    std::free(str);
    str = Translation(-1);
    EXPECT_EQ(str, nullptr);
    std::free(str);
}