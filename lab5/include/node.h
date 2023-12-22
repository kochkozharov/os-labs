#pragma once

#include "memory"
#include "socket.h"

class ControlNode {
   private:
    Socket sock;
    ControlNode();
    ControlNode(ControlNode const &) = delete;
    ControlNode &operator=(ControlNode const &) = delete;
   public:
    static ControlNode &get();
    void send(int id, const std::string &msg);
    std::optional<std::string> recieve();
};

class ComputationNode {
   private:
    Socket sock;
    int id;

   public:
    void compute();
    ComputationNode(int id, int parentId);
    ~ComputationNode();
};