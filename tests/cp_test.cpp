#include <gtest/gtest.h>
#include "utils.h"


TEST(Lab5Tests, CalculationTest) {
    ASSERT_EQ(MakeGuess(1234, 1234), GuessResult(4, 0));
    ASSERT_EQ(MakeGuess(1243, 1234), GuessResult(2, 2));
    ASSERT_EQ(MakeGuess(1243, 9847), GuessResult(1, 0));
    ASSERT_EQ(MakeGuess(1243, 9147), GuessResult(1, 1));
    ASSERT_EQ(MakeGuess(2301, 127), GuessResult(0, 3));
}