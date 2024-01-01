#include <format>
#include <queue>
#include <vector>
#include <csignal>
#include <iostream>

#include "shared_memory.h"
#include "thread"
#include "utils.h"

void GameLoop(SharedMemory &gameMemory, int maxSlots) {
    srand(time(nullptr));
    int mysteryNumber = rand() % 4000;
    /*
    WeakSharedMemory gameMemory(
        "BC" + std::to_string(gameID),
        sizeof(int) + maxSlots * sizeof(ConnectionSlot));
    */
    auto *statusPtr = static_cast<int *>(gameMemory.getData());
    auto *gamePtr = reinterpret_cast<ConnectionSlot *>(statusPtr + 1);
    bool guessed = false;
    while (!guessed) {
        gameMemory.readLock();
        auto which = *statusPtr;
        auto res = MakeGuess(mysteryNumber, gamePtr[which].guess);
        auto outputStr = std::format(
            "Player {}:\n \tGuess {} \tBulls {} \t Cows {}", gamePtr[which].pid,
            gamePtr[which].guess, res.bulls, res.cows);
        for (int i = 0; i < maxSlots; ++i) {
            auto pid = gamePtr[i].pid;
            auto fd =
                open((std::format("/proc/{}/fd/0", pid)).c_str(), O_WRONLY);
            write(fd, outputStr.c_str(), outputStr.size() + 1);
        }
        if (res == GuessResult(4, 0)) {
            guessed = true;
        }
        gameMemory.writeUnlock();
    }
    for (int i = 0; i < maxSlots; ++i) {
        auto pid = gamePtr[i].pid;
        kill(pid, SIGTERM);
    }
}

int main() {
    SharedMemory req(REQUEST_SLOT_NAME, sizeof(Request));
    SharedMemory rep(RESPONSE_SLOT_NAME, sizeof(Response));
    auto *reqPtr = static_cast<Request *>(req.getData());
    auto *repPtr = static_cast<Response *>(rep.getData());
    std::priority_queue<Game> pq;
    std::vector<SharedMemory> games;
    std::vector<std::thread> threads;
    int gamesCount = 0;

    int gameID;
    int maxSlots;
    while (true) {
        req.readLock();

        if (reqPtr->newGame) {
            pq.emplace(gamesCount, reqPtr->maxSlots, reqPtr->maxSlots);
            gameID = gamesCount;
            maxSlots = reqPtr->maxSlots;
            gamesCount++;
            std::cerr << "LOL\n";
            games.emplace_back("BC" + std::to_string(gameID),
                               sizeof(int) + maxSlots * sizeof(ConnectionSlot));
            threads.emplace_back(GameLoop, std::ref(games[gameID]), maxSlots);
        } else {
            auto freeGame = pq.top();
            if (freeGame.freeSlots == 0) {
                gameID = -1;
            } else {
                gameID = freeGame.gameID;
                pq.pop();
                freeGame.freeSlots--;
                pq.push(freeGame);
                maxSlots = freeGame.freeSlots;
            }
        }
        rep.writeLock();
        req.writeUnlock();
        repPtr->maxSlots = maxSlots;
        repPtr->gameID = gameID;
        rep.readUnlock();
    }
}