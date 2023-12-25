#pragma once
#include <optional>
#include <string>
#include <zmq.hpp>

class Socket {
   private:
    zmq::context_t ctx;
    zmq::socket_t sock ;

   public:
    Socket(zmq::socket_type t);
    void bind(int id);
    void unbind(int id);
    bool connect(int id);
    void disconnect(int id);
    bool sendMessage(const std::string &msg);
    std::optional<std::string> receiveMessage(bool nowait);
};
