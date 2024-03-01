#include "node.h"

#include "iostream"
#include "stdexcept"

ControlNode::ControlNode() : sock(zmq::socket_type::pair) {
    sock.setTimeout();
}

ControlNode &ControlNode::get() {
    static ControlNode instance;
    return instance;
}

bool ControlNode::send(int id, const std::string &msg) {
    auto status = sock.connect(id) && sock.sendMessage(msg);
    prevConId = id;
    return status;
}

std::optional<std::string> ControlNode::receive() {
    auto temp = sock.receiveMessage(false);
    sock.disconnect(prevConId);
    return temp;

}

ComputationNode::ComputationNode(int id)
    : sock(zmq::socket_type::pair), id(id) {
    sock.bind(id);
}

ComputationNode::~ComputationNode() { sock.unbind(id); }

std::string ComputationNode::findAllOccurencies(const std::string &hay,
                                                const std::string &needle) {
    std::string repMsg = "";
    std::size_t pos = hay.find(needle, 0);
    while (pos != std::string::npos) {
        repMsg += std::to_string(pos) + ';';
        pos = hay.find(needle, pos + 1);
    }
    repMsg.pop_back();
    return repMsg;
}

void ComputationNode::computationLoop() {
    while (true) {
        auto reqMsg = sock.receiveMessage(false);
        std::stringstream ss(reqMsg.value());
        std::string command;
        ss >> command;
        if (command == "exec") {
            std::string hay, needle;
            ss >> hay >> needle;
            sock.sendMessage("Ok: " + std::to_string(id) + ' ' +
                             findAllOccurencies(hay, needle));
        } else if (command == "ping") {
            sock.sendMessage("pong");
        }
    }
}
