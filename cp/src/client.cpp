#include <unistd.h>

#include <iostream>
#include <vector>

#include "shared_memory.h"
#include "utils.h"

int main() {
    WeakSharedMemory req(REQUEST_SLOT_NAME, sizeof(Request));
    WeakSharedMemory rep(RESPONSE_SLOT_NAME, sizeof(Response));
    auto *reqPtr = static_cast<Request *>(req.getData());
    auto *repPtr = static_cast<Response *>(rep.getData());
    int gameID;
    int maxSlots;
    std::string command;
    std::cout << "> ";
    while (std::cin >> command) {
        if (command == "create") {
            std::cin >> maxSlots;
            req.writeLock();
            reqPtr->newGame = true;
            reqPtr->pid = getpid();
            reqPtr->maxSlots = maxSlots;
            rep.readLock();
            req.readUnlock();
            gameID = repPtr->gameID;
            rep.writeUnlock();
        } else if (command == "connect") {
            req.writeLock();
            reqPtr->newGame = false;
            reqPtr->pid = getpid();
            rep.readLock();
            req.readUnlock();
            gameID = repPtr->gameID;
            maxSlots = repPtr->maxSlots;
            rep.writeUnlock();
        } else {
            std::cerr << "Unknown command\n";
        }
        std::cerr << "> ";
        if (gameID != -1) {
            break;
        }
        std::cerr << "No free games available. Try creating new\n";
    }
    std::cerr << "here!\n";

    WeakSharedMemory gameMemory(
        "BC" + std::to_string(gameID),
        sizeof(int) + maxSlots * sizeof(ConnectionSlot));
    auto *statusPtr = static_cast<int *>(gameMemory.getData());
    auto *gamePtr = reinterpret_cast<ConnectionSlot *>(statusPtr + 1);
    while (true) {
        int guess;
        std::cin >> guess;
        if (guess >= 4000 || guess < 0) {
            std::cerr << "Incorrect format\n";
            continue;
        }
        gameMemory.writeLock();
        *statusPtr = gameID;
        gamePtr[gameID].pid = getpid();
        gamePtr[gameID].guess = guess;
        gameMemory.readUnlock();
        std::cout << "> ";
    }
}