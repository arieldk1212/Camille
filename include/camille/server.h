#ifndef CAMILLE_INCLUDE_CAMILLE_SERVER_H_
#define CAMILLE_INCLUDE_CAMILLE_SERVER_H_

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <print>

namespace server {

constexpr int PORT = 4354;
static_assert(PORT == 4354, "Server Port must be 4354!");

class Server {
 public:
  Server()
      : serverfd_(socket(AF_INET, SOCK_STREAM, 0)),
        server_addr_({AF_INET, htons(PORT), INADDR_ANY}) {
    if (serverfd_ == -1) {
      std::println("Socket Initialization Error..");
    }

    if (bind(serverfd_, (struct sockaddr*)&server_addr_, sizeof(server_addr_)) < 0) {
      std::println("Binding Failed..");
    }
    std::println("Binding Completed..");
  }

  void Listen() const {
    if (listen(serverfd_, 5) < 0) {
      std::println("Listening Failed..");
    }
    std::println("Started Listening on Port 4354..");
  }

 private:
  int serverfd_;
  struct sockaddr_in server_addr_;
};

class Client {
 public:
 private:
};

class SocketHandler {};

};  // namespace server

#endif