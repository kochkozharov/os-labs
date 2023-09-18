#include <gtest/gtest.h>

extern "C" {
#include <test.h>
}

TEST(FirstLabTests, SimpleTest) {
    CreateProcess();
    ASSERT_TRUE(true);
}