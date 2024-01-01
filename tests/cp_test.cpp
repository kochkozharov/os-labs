#include <gtest/gtest.h>

#include "shared_memory.h"
#include "utils.h"

TEST(Lab5Tests, CalculationTest) {
    ASSERT_EQ(MakeGuess(1234, 1234), GuessResult(4, 0));
    ASSERT_EQ(MakeGuess(1243, 1234), GuessResult(2, 2));
    ASSERT_EQ(MakeGuess(1243, 9847), GuessResult(1, 0));
    ASSERT_EQ(MakeGuess(1243, 9147), GuessResult(1, 1));
    ASSERT_EQ(MakeGuess(2301, 127), GuessResult(0, 3));
}

TEST(Lab5Tests, SharedMemoryTest) {
    SharedMemory a("test", 10);
    for (int i = 0; i < 10; ++i) {
        static_cast<char *>(a.getData())[i] = i + '0';
    }
    auto pid = fork();
    if (pid == 0) {
        WeakSharedMemory b("test", 10);
        for (int i = 0; i < 10; ++i) {
            EXPECT_EQ(static_cast<char *>(b.getData())[i], i + '0');
        }
        exit(EXIT_SUCCESS);
    }

    wait(nullptr);
}

TEST(Lab5Tests, lockTest) {
    auto pid = fork();
    if (pid == 0) {
        while (true) {
            WeakSharedMemory b("test", 10);
            for (int i = 0; i < 10; ++i) {
                static_cast<char *>(b.getData())[i] = i + '0';
            }
            b.readUnlock();
            exit(EXIT_SUCCESS);
        }
    }
    SharedMemory a("test", 10);
    a.readLock();
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(static_cast<char *>(a.getData())[i], i + '0');
    }

}