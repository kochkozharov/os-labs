#include <socket.h>
#include <zmq.h>

#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
constexpr int MAIN_PORT = 5050;

static std::string GetAddress(int sockId) {
    return "tcp://localhost:" + std::to_string(MAIN_PORT + sockId);
}

Socket::Socket(int type) : sock(ctx, type) {}

void Socket::connect(int id) { sock.connect(GetAddress(id)); }

void Socket::disconnect(int id) { sock.disconnect(GetAddress(id)); }

void Socket::bind(int id) {
    try {
        sock.bind(GetAddress(id));
    } catch (...) {
        throw std::runtime_error("Error: port already in use");
    }
}

void Socket::unbind(int id) { sock.unbind(GetAddress(id)); }

void Socket::sendMessage(const std::string &msg) {
    zmq::message_t zmsg(msg.size());
    memcpy(zmsg.data(), msg.c_str(), msg.size());
    sock.send(zmsg, zmq::send_flags::none);
}
std::optional<std::string> Socket::recieveMessage(bool nowait) {
    zmq::message_t zmsg;
    auto len = sock.recv(
        zmsg, nowait ? zmq::recv_flags::dontwait : zmq::recv_flags::none);
    if (len.has_value()) {
        std::string receivedMsg(static_cast<char *>(zmsg.data()), len.value());
        return receivedMsg;
    }
    return {};
}