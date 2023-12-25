#include "node.h"

#include "stdexcept"

ControlNode::ControlNode() : sock(zmq::socket_type::req) {

}

ControlNode &ControlNode::get() {
    static ControlNode instance;
    return instance;
}

bool ControlNode::send(int id, const std::string &msg) {
    auto status = sock.connect(id) && sock.sendMessage(msg);
    // sock.disconnect(id);
    return status;
}

std::optional<std::string> ControlNode::recieve() {
    return sock.recieveMessage(false);
}

ComputationNode::ComputationNode(int id) : sock(zmq::socket_type::rep), id(id) {
    // Topology::get().insert(id, parentId);
    sock.bind(id);
}

ComputationNode::~ComputationNode() { sock.unbind(id); }

void ComputationNode::computationLoop() {
    while (true) {
        auto reqMsg = sock.recieveMessage(false);
        std::stringstream ss(reqMsg.value());
        std::string command;
        ss >> command;
        if (command == "exec") {
            std::string repMsg = "Ok: " + std::to_string(id) + ": ";
            std::string hay, needle;
            ss >> hay >> needle;

            std::size_t pos = hay.find(needle, 0);
            while (pos != std::string::npos) {
                repMsg += std::to_string(pos) + ';';
                pos = hay.find(needle, pos + 1);
            }
            repMsg.pop_back();
            sock.sendMessage(repMsg);
        } else if (command == "ping") {
            sock.sendMessage("pong");
        }
        else if (command == "kill") {
            sock.sendMessage("killed");
            break;
        }
    }
}

bool ControlNode::tryConnect(int id) {
    try {
        sock.connect(id);
    } catch (...) {
        return false;
    }
    return true;
}