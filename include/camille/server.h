#ifndef CAMILLE_INCLUDE_CAMILLE_SERVER_H_
#define CAMILLE_INCLUDE_CAMILLE_SERVER_H_

#include "socket_types.h"
#include "stream.h"

#include <print>
#include <atomic>
#include <memory>
#include <functional>

namespace camille {

// Default configuration
constexpr int kDefaultPort = 4354;
constexpr int kDefaultBacklog = 128;  // cpp-httplib uses higher backlog

// Connection handler callback type (cpp-httplib style)
using ConnectionHandler = std::function<void(std::unique_ptr<SocketStream>)>;

class Server {
 public:
  // cpp-httplib style: configurable host and port
  explicit Server(const std::string& host = "0.0.0.0", int port = kDefaultPort)
      : host_(host),
        port_(port),
        svr_sock_(kInvalidSocket),
        is_running_(false) {}

  ~Server() {
    stop();
  }

  // Non-copyable (cpp-httplib style)
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  // Movable
  Server(Server&& other) noexcept
      : host_(std::move(other.host_)),
        port_(other.port_),
        socket_options_(other.socket_options_),
        timeouts_(other.timeouts_),
        svr_sock_(other.svr_sock_.load()),
        is_running_(other.is_running_.load()) {
    other.svr_sock_ = kInvalidSocket;
    other.is_running_ = false;
  }

  Server& operator=(Server&& other) noexcept {
    if (this != &other) {
      stop();
      host_ = std::move(other.host_);
      port_ = other.port_;
      socket_options_ = other.socket_options_;
      timeouts_ = other.timeouts_;
      svr_sock_ = other.svr_sock_.load();
      is_running_ = other.is_running_.load();
      other.svr_sock_ = kInvalidSocket;
      other.is_running_ = false;
    }
    return *this;
  }

  // cpp-httplib style: chainable configuration setters
  Server& set_socket_options(const SocketOptions& opts) {
    socket_options_ = opts;
    return *this;
  }

  Server& set_tcp_nodelay(bool on) {
    socket_options_.tcp_nodelay = on;
    return *this;
  }

  Server& set_read_timeout(std::chrono::seconds timeout) {
    timeouts_.read_timeout = timeout;
    return *this;
  }

  Server& set_write_timeout(std::chrono::seconds timeout) {
    timeouts_.write_timeout = timeout;
    return *this;
  }

  Server& set_connection_timeout(std::chrono::seconds timeout) {
    timeouts_.connection_timeout = timeout;
    return *this;
  }

  Server& set_connection_handler(ConnectionHandler handler) {
    connection_handler_ = std::move(handler);
    return *this;
  }

  // Bind to port (cpp-httplib style: separate bind from listen)
  bool bind_to_port(int backlog = kDefaultBacklog) {
    svr_sock_ = create_server_socket();
    if (svr_sock_ == kInvalidSocket) {
      return false;
    }

    if (!apply_socket_options(svr_sock_, socket_options_)) {
      close_socket(svr_sock_);
      svr_sock_ = kInvalidSocket;
      return false;
    }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port_));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(svr_sock_, reinterpret_cast<struct sockaddr*>(&addr),
               sizeof(addr)) < 0) {
      close_socket(svr_sock_);
      svr_sock_ = kInvalidSocket;
      return false;
    }

    if (::listen(svr_sock_, backlog) < 0) {
      close_socket(svr_sock_);
      svr_sock_ = kInvalidSocket;
      return false;
    }

    std::println("Server bound to {}:{}", host_, port_);
    return true;
  }

  // cpp-httplib style: blocking listen loop
  bool listen_after_bind() {
    is_running_ = true;
    std::println("Server listening on port {}", port_);

    while (is_running_) {
      if (!wait_until_socket_is_ready(svr_sock_, std::chrono::milliseconds(100),
                                       true, false)) {
        continue;  // Timeout, check if still running
      }

      struct sockaddr_in client_addr{};
      socklen_t addr_len = sizeof(client_addr);
      socket_t client_sock = accept(
          svr_sock_,
          reinterpret_cast<struct sockaddr*>(&client_addr),
          &addr_len);

      if (client_sock == kInvalidSocket) {
        if (is_running_) {
          std::println("Accept failed");
        }
        continue;
      }

      // Create stream and handle connection (cpp-httplib style)
      auto stream = std::make_unique<SocketStream>(client_sock, timeouts_);
      process_and_close_socket(std::move(stream));
    }

    return true;
  }

  // Convenience: bind + listen in one call
  bool listen() {
    if (!bind_to_port()) {
      return false;
    }
    return listen_after_bind();
  }

  void stop() {
    is_running_ = false;
    socket_t sock = svr_sock_.exchange(kInvalidSocket);
    if (sock != kInvalidSocket) {
      close_socket(sock);
      std::println("Server socket closed");
    }
  }

  [[nodiscard]] bool is_running() const { return is_running_; }
  [[nodiscard]] bool is_valid() const { return svr_sock_ != kInvalidSocket; }
  [[nodiscard]] int port() const { return port_; }

 private:
  static socket_t create_server_socket() {
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == kInvalidSocket) {
      std::println("Socket creation failed");
    }
    return sock;
  }

  // cpp-httplib style: process connection then close
  void process_and_close_socket(std::unique_ptr<SocketStream> stream) {
    if (connection_handler_) {
      connection_handler_(std::move(stream));
    } else {
      // Default: just close
      std::string ip;
      int port;
      stream->get_remote_ip_and_port(ip, port);
      std::println("Connection from {}:{} (no handler set)", ip, port);
    }
  }

  std::string host_;
  int port_;
  SocketOptions socket_options_;
  TimeoutConfig timeouts_;
  std::atomic<socket_t> svr_sock_;
  std::atomic<bool> is_running_;
  ConnectionHandler connection_handler_;
};

// Client connection wrapper (cpp-httplib style)
class Client {
 public:
  explicit Client(const std::string& host, int port)
      : host_(host), port_(port), sock_(kInvalidSocket) {}

  ~Client() {
    close();
  }

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  Client(Client&& other) noexcept
      : host_(std::move(other.host_)),
        port_(other.port_),
        socket_options_(other.socket_options_),
        timeouts_(other.timeouts_),
        sock_(other.sock_) {
    other.sock_ = kInvalidSocket;
  }

  Client& operator=(Client&& other) noexcept {
    if (this != &other) {
      close();
      host_ = std::move(other.host_);
      port_ = other.port_;
      socket_options_ = other.socket_options_;
      timeouts_ = other.timeouts_;
      sock_ = other.sock_;
      other.sock_ = kInvalidSocket;
    }
    return *this;
  }

  // cpp-httplib style: chainable setters
  Client& set_tcp_nodelay(bool on) {
    socket_options_.tcp_nodelay = on;
    return *this;
  }

  Client& set_connection_timeout(std::chrono::seconds timeout) {
    timeouts_.connection_timeout = timeout;
    return *this;
  }

  Client& set_read_timeout(std::chrono::seconds timeout) {
    timeouts_.read_timeout = timeout;
    return *this;
  }

  Client& set_write_timeout(std::chrono::seconds timeout) {
    timeouts_.write_timeout = timeout;
    return *this;
  }

  // Connect to server
  Error connect() {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ == kInvalidSocket) {
      return Error::Connection;
    }

    apply_socket_options(sock_, socket_options_);

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) <= 0) {
      close();
      return Error::Connection;
    }

    if (::connect(sock_, reinterpret_cast<struct sockaddr*>(&addr),
                  sizeof(addr)) < 0) {
      close();
      return Error::Connection;
    }

    return Error::Success;
  }

  // Get stream for I/O operations
  std::unique_ptr<SocketStream> stream() {
    if (sock_ == kInvalidSocket) return nullptr;
    return std::make_unique<SocketStream>(sock_, timeouts_);
  }

  void close() {
    if (sock_ != kInvalidSocket) {
      close_socket(sock_);
      sock_ = kInvalidSocket;
    }
  }

  [[nodiscard]] bool is_connected() const { return sock_ != kInvalidSocket; }

 private:
  std::string host_;
  int port_;
  SocketOptions socket_options_;
  TimeoutConfig timeouts_;
  socket_t sock_;
};

}  // namespace camille

#endif
