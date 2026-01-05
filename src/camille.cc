#include <cstdio>
#include <memory>
#include <asio.hpp>

using asio::ip::tcp;

asio::io_context io_context;
constexpr size_t port = 6970;
asio::ip::tcp::acceptor server(io_context, tcp::endpoint(tcp::v4(), port));

class Request {
 public:
  explicit Request(tcp::socket&& socket)
      : socket(std::move(socket)) {}

  tcp::socket socket;
  asio::streambuf buffer;
};

void handle_request(const std::error_code& error, tcp::socket socket) {
  server.async_accept(io_context, &handle_request);  // The Loop

  if (error) {
    std::println("Error while accepting: %s\n", error.message().c_str());
    return;
  }

  auto request = std::make_shared<Request>(std::move(socket));

  std::println("Accepted connection\n");

  asio::async_read_until(
      request->socket, request->buffer, '\n',
      [request](const auto& error, size_t bytes_transferred) {
        if (error) {
          std::println("Error while reading: %s\n", error.message().c_str());
          return;
        }
        std::println("Received %zu bytes\n", bytes_transferred);
        asio::async_write(request->socket, request->buffer,
                          [request](const auto& error, size_t bytes_transferred) {
                            if (error) {
                              std::println("Error while writing: %s\n", error.message().c_str());
                              return;
                            }
                            std::println("Sent %zu bytes\n", bytes_transferred);
                          });
      });
}

int main() {
  size_t seconds = 5;
  asio::steady_timer t(io_context, asio::chrono::seconds(seconds));
  std::println("Waiting for %zu seconds\n", seconds);
  t.async_wait([seconds](const std::error_code& e) {
    std::println("------------------------------\n");
    std::println("Done waiting %zu seconds. Error: %s\n", seconds, e.message().c_str());
    std::println("------------------------------\n");
  });

  std::println("Listening too 127.0.0.1:%zu\n", port);
  server.async_accept(io_context, &handle_request);
  io_context.run();

  return 0;
}