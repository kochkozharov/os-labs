#include <unistd.h>

#include <iostream>

#include "node.h"
#include "sys/wait.h"
#include "topology.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Not enough arguments\n";
        std::exit(EXIT_FAILURE);
    }
    int forksCount = 0;
    std::string command;
    std::cout << "> ";
    while (std::cin >> command) {
        if (command == "create") {
            int id, parentId;
            std::cin >> id >> parentId;
            if (!Topology::get().insert(id, parentId)) {
                std::cout << "> ";
                std::cout.flush();
                continue;
            }
            forksCount++;
            pid_t pid = fork();
            if (pid == -1) {
                std::perror("fork");
                std::exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                execl(argv[1], argv[1], std::to_string(id).c_str(),
                      std::to_string(parentId).c_str(), nullptr);

            } else {
                std::cout << "Ok: " + std::to_string(pid) + '\n';
            }
            std::cout << "> ";
            std::cout.flush();
            continue;
        } else if (command == "ping") {
            int id;
            std::cin >> id;
            if (Topology::get().find(id) == Topology::get().end()) {
                std::cerr << "Error: Not found\n";
                std::cout << "> ";
                std::cout.flush();
                continue;
            }
            if (ControlNode::get().tryConnect(id)) {
                std::cout << "Ok: 1\n";
            } else {
                std::cout << "Ok: 0\n";
            }
        } else if (command == "exec") {
            int id;

            std::string hay, needle;
            std::cin >> id >> hay >> needle;
            if (Topology::get().find(id) == Topology::get().end()) {
                std::cerr << "Error: " + std::to_string(id) + " Not found\n";
                std::cout << "> ";
                std::cout.flush();
                continue;
            }
            if (!ControlNode::get().send(id, "exec " + hay + ' ' + needle)) {
                std::cerr << "Error: Node is unavailable";
            }

        } else if (command == "kill") {
            int id;
            std::cin >> id;
            if (Topology::get().erase(id)) {
                if (ControlNode::get().send(id, "kill"))
                    ControlNode::get().recieve();

            } else if (command == "exit") {
                auto iter = Topology::get().begin();
                auto end = Topology::get().end();

                for (; iter.it1 != end.it1; iter.it1++) {
                    for (auto id : *iter.it1) {
                        if (ControlNode::get().send(id, "kill"))
                            ControlNode::get().recieve();
                    }
                }
            }
            break;
        } else {
            std::cout << "Unknown command\n";
            std::cout << "> ";
            std::cout.flush();
            continue;
        }
        auto responce = ControlNode::get().recieve();
        if (responce) {
            std::cout << responce.value() << '\n';
        }
        std::cout << "> ";
        std::cout.flush();
    }

    for (int i = 0; i < forksCount; ++i) {
        wait(nullptr);
    }
}