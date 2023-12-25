#pragma once

#include "memory"
#include "socket.h"

constexpr int CTRL_NODE_ID = -1;

class ControlNode {
   private:
    Socket sock;
    ControlNode();
    ControlNode(ControlNode const &) = delete;
    ControlNode &operator=(ControlNode const &) = delete;

   public:
    static ControlNode &get();
    bool tryConnect(int id);
    bool send(int id, const std::string &msg);
    std::optional<std::string> recieve();
};

class ComputationNode {
   private:
    Socket sock;
    int id;

   public:
    static std::string findAllOccurencies(const std::string &hay, const std::string &needle);
    void computationLoop();
    ComputationNode(int id);
    ~ComputationNode();
};