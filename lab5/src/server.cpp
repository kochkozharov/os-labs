#include <node.h>
#include <iostream>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Not enough arguments\n";
        std::exit(EXIT_FAILURE);
    }
    int id = std::stoi(argv[1]);
    ComputationNode cn(id);
    cn.computationLoop();
    return 0;
}