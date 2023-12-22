#pragma once
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <zmq.hpp>

class Socket {
   private:
    zmq::context_t ctx;
    zmq::socket_t sock;

   public:
    Socket(int type);
    void bind(int id);
    void unbind(int id);
    void connect(int id);
    void disconnect(int id);
    void sendMessage(const std::string &msg);
    std::optional<std::string> recieveMessage(bool nowait);
};
