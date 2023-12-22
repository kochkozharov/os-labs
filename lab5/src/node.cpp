#include "node.h"

#include "stdexcept"
#include "topology.h"

constexpr int CTRL_NODE_ID = -1;

ControlNode &ControlNode::get() {
    static ControlNode instance;
    return instance;
}

ControlNode::ControlNode() : sock(ZMQ_REQ) {}

void ControlNode::send(int id, const std::string &msg) {
    try {
        sock.connect(id);
    } catch (...) {
        Topology::get().erase(id);
        throw std::runtime_error("Error: " + std::to_string(id) +
                                 " Node is unavailable");
    }
    sock.sendMessage(msg);
    sock.disconnect(id);
}

std::optional<std::string> ControlNode::recieve() {
    return sock.recieveMessage(true);
}

ComputationNode::ComputationNode(int id, int parentId) : sock(ZMQ_REP), id(id) {
    if (id == CTRL_NODE_ID) {
        throw std::invalid_argument("Error: Already exists");
    }
    Topology::get().insert(id, parentId);
    sock.bind(id);
}

ComputationNode::~ComputationNode() { sock.unbind(id); }


void ComputationNode::compute() {
    auto reqMsg = sock.recieveMessage(false);
    std::string repMsg;
    sock.sendMessage(repMsg);
}
