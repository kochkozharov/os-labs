#include <socket.h>

#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

static std::string GetAddress(int sockId) {
    constexpr int MAIN_PORT = 4000;
    return "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + sockId);
}

Socket::Socket(zmq::socket_type t) : sock(ctx, t) {}

bool Socket::connect(int id) {
    try {
        sock.connect(GetAddress(id));
    } catch (...) {
        return false;
    }
    return true;
}

void Socket::disconnect(int id) { sock.disconnect(GetAddress(id)); }

void Socket::bind(int id) {
    try {
        sock.bind(GetAddress(id));
    } catch (...) {
        throw std::runtime_error("Error: port already in use");
    }
}

void Socket::unbind(int id) { sock.unbind(GetAddress(id)); }

bool Socket::sendMessage(const std::string &msg) {
    return sock.send(zmq::buffer(msg), zmq::send_flags::none).has_value();
}
std::optional<std::string> Socket::receiveMessage(bool nowait) {
    zmq::message_t zmsg{};
    auto len = sock.recv(
        zmsg, nowait ? zmq::recv_flags::dontwait : zmq::recv_flags::none);
    if (len) {
        return zmsg.to_string();
    }
    return {};
}