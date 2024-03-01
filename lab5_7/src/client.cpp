#include <unistd.h>

#include <iostream>

#include "node.h"
#include "topology.h"
 #include <signal.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Not enough arguments\n";
        std::exit(EXIT_FAILURE);
    }
    std::string command;
    std::cout << "> ";

    Topology topo;

    while (std::cin >> command) {
        if (command == "create") {
            int id, parentId;
            std::cin >> id >> parentId;

            pid_t pid = fork();
            if (pid == -1) {
                std::perror("fork");
                std::exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                execl(argv[1], argv[1], std::to_string(id).c_str(),
                      std::to_string(parentId).c_str(), nullptr);

            } else {
                if (!topo.insert(Topology::NodeId(id, pid), parentId)) {
                    std::cout << "> ";
                    std::cout.flush();
                    continue;
                }
                std::cout << "Ok: " + std::to_string(pid) + '\n';
            }
        } else if (command == "ping") {
            int id;
            std::cin >> id;
            if (!topo.contains(id)) {
                std::cerr << "Error: Not found\n";
                std::cout << "> ";
                std::cout.flush();
                continue;
            }
            if (!ControlNode::get().send(id, "ping")) {
                std::cout << "Ok: 0\n";
                std::cout << "> ";
                std::cout.flush();
                continue;
            }
            auto response = ControlNode::get().receive();
            if (response == "pong") {
                std::cerr << "Ok: 1\n";
            } else {
                std::cerr << "Ok: 0\n";
            }
        } else if (command == "exec") {
            int id;

            std::string hay, needle;
            std::cin >> id >> hay >> needle;
            if (!topo.contains(id)) {
                std::cerr << "Error: " + std::to_string(id) + " Not found\n";
                std::cout << "> ";
                std::cout.flush();
                continue;
            }
            if (!ControlNode::get().send(id, "exec " + hay + ' ' + needle)) {
                std::cerr << "Error: Node is unavailable";
                std::cout << "> ";
                std::cout.flush();
                continue;
            }
            auto response = ControlNode::get().receive();
            if (response) {
                std::cout << *response << '\n';
            }
        } else {
            std::cout << "Unknown command\n";
        }

        std::cout << "> ";
        std::cout.flush();
    }
    auto it = topo.begin();
    auto end = topo.end();
    for (;it.it1 != end.it1; it.it1++) {
        it.it2 = it.it1->begin();
        for (;it.it2 != it.it1->end(); it.it2++) {
            kill(it.it2->pid, SIGKILL);
        }
    }
}