#include "utils.h"

#include <sys/types.h>

#include <array>

#include "shared_memory.h"
#include "unistd.h"

bool operator==(const GuessResult &a, const GuessResult &b) {
    return (a.bulls == b.bulls) && (a.cows == b.cows);
}

GuessResult MakeGuess(int secret, int guess) {  // w/o checks
    int bullsCnt = 0;
    int cowsCnt = 0;
    constexpr int base = 10;
    std::array<int, base> secretArray{};
    std::array<int, base> guessArray{};
    for (int i = 0; i < 4; ++i) {
        auto c1 = secret % base;
        auto c2 = guess % base;
        if (c1 == c2) {
            ++bullsCnt;
        } else {
            ++secretArray[c1];
            ++guessArray[c2];
        }
        secret /= base;
        guess /= base;
    }
    for (int i = 0; i < base; ++i) {
        cowsCnt += std::min(secretArray[i], guessArray[i]);
    }
    return {bullsCnt, cowsCnt};
}

bool operator<(const Game &a, const Game &b) {
    return a.freeSlots < b.freeSlots;
}

