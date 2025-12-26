#include "camille/camille.h"

#include <print>
#include <string>

int main() {
  // cpp-httplib style: chainable configuration
  camille::Server server("0.0.0.0", 4354);

  server.set_tcp_nodelay(true)
      .set_read_timeout(std::chrono::seconds(30))
      .set_write_timeout(std::chrono::seconds(30))
      .set_connection_handler([](std::unique_ptr<camille::SocketStream> stream) {
        // Get client info
        std::string client_ip;
        int client_port = 0;
        stream->get_remote_ip_and_port(client_ip, client_port);
        std::println("Client connected: {}:{}", client_ip, client_port);

        // Echo server example: read and write back
        char buffer[1024];
        auto bytes_read = stream->read(buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
          buffer[bytes_read] = '\0';
          std::println("Received: {}", buffer);
          stream->write(buffer, static_cast<size_t>(bytes_read));
        }

        std::println("Client disconnected: {}:{}", client_ip, client_port);
      });

  // Start listening (blocks)
  if (!server.listen()) {
    std::println("Failed to start server");
    return 1;
  }

  return 0;
}