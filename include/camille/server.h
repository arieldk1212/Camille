#ifndef CAMILLE_INCLUDE_CAMILLE_SERVER_H_
#define CAMILLE_INCLUDE_CAMILLE_SERVER_H_

#include "socket_types.h"
#include "stream.h"

#include <print>
#include <atomic>
#include <memory>
#include <functional>

namespace camille {

inline constexpr int kDefaultPort = 4354;
inline constexpr int kDefaultBacklog = 128;

using ConnectionHandler = std::function<void(std::unique_ptr<SocketStream>)>;

class Server {
 public:
  explicit Server(const std::optional<std::string>& host, int port = kDefaultPort)
      : host_(host.has_value() && !host->empty() ? *host : std::string("0.0.0.0")),
        port_(port),
        svr_sock_(kInvalidSocket),
        is_running_(false) {}

  ~Server() { Stop(); }

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

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
      Stop();
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

  Server& SetSocketOption(const socket_types::SocketOptions& opts) {
    socket_options_ = opts;
    return *this;
  }

  Server& SetTCPNodelay(bool status) {
    socket_options_.tcp_nodelay = status;
    return *this;
  }

  Server& SetReadTimeout(std::chrono::seconds timeout) {
    timeouts_.read_timeout = timeout;
    return *this;
  }

  Server& SetWriteTimeout(std::chrono::seconds timeout) {
    timeouts_.write_timeout = timeout;
    return *this;
  }

  Server& SetConnectionTimeout(std::chrono::seconds timeout) {
    timeouts_.connection_timeout = timeout;
    return *this;
  }

  Server& SetConnectionHandler(ConnectionHandler handler) {
    connection_handler_ = std::move(handler);
    return *this;
  }

  bool BindToPort(int backlog = kDefaultBacklog) {
    svr_sock_ = CreateServerSocket();
    if (svr_sock_ == kInvalidSocket) {
      return false;
    }

    if (!socket_types::ApplySocketOptions(svr_sock_, socket_options_)) {
      socket_types::CloseSocket(svr_sock_);
      svr_sock_ = kInvalidSocket;
      return false;
    }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port_));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(svr_sock_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
      socket_types::CloseSocket(svr_sock_);
      svr_sock_ = kInvalidSocket;
      return false;
    }

    if (::listen(svr_sock_, backlog) < 0) {
      socket_types::CloseSocket(svr_sock_);
      svr_sock_ = kInvalidSocket;
      return false;
    }

    std::println("Server running on {}:{}", host_, port_);
    return true;
  }

  bool ListenAfterBind() {
    is_running_ = true;
    std::println("Listening..");

    while (is_running_) {
      if (!socket_types::WaitUntilSocketReady(svr_sock_, std::chrono::milliseconds(100), true,
                                              false)) {
        continue;
      }

      struct sockaddr_in client_addr{};
      socklen_t addr_len = sizeof(client_addr);
      socket_t client_sock =
          accept(svr_sock_, reinterpret_cast<struct sockaddr*>(&client_addr), &addr_len);

      if (client_sock == kInvalidSocket) {
        if (is_running_) {
          std::println("Accept failed");
        }
        continue;
      }

      auto stream = std::make_unique<SocketStream>(client_sock, timeouts_);
      ProcessAndCloseSocket(std::move(stream));
    }

    return true;
  }

  bool Listen() {
    if (!BindToPort()) {
      return false;
    }
    return ListenAfterBind();
  }

  void Stop() {
    is_running_ = false;
    socket_t sock = svr_sock_.exchange(kInvalidSocket);
    if (sock != kInvalidSocket) {
      socket_types::CloseSocket(sock);
      std::println("Server socket closed");
    }
  }

  [[nodiscard]] bool IsRunning() const { return is_running_; }
  [[nodiscard]] bool IsValid() const { return svr_sock_ != kInvalidSocket; }
  [[nodiscard]] int Port() const { return port_; }

 private:
  static socket_t CreateServerSocket() {
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == kInvalidSocket) {
      std::println("Socket creation failed");
    }
    return sock;
  }

  void ProcessAndCloseSocket(std::unique_ptr<SocketStream> stream) {
    if (connection_handler_) {
      connection_handler_(std::move(stream));
    } else {
      std::string ip_address;
      int port = 0;
      stream->GetRemoteIpAndPort(ip_address, port);
      std::println("Connection from {}:{} (no handler set)", ip_address, port);
    }
  }

  int port_;
  std::string host_;
  std::atomic<bool> is_running_;
  std::atomic<socket_t> svr_sock_;
  socket_types::TimeoutConfig timeouts_;
  ConnectionHandler connection_handler_;
  socket_types::SocketOptions socket_options_;
};

class Client {
 public:
  Client(std::string& host, int port)
      : host_(std::move(host)),
        port_(port) {}

  ~Client() { Close(); }

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
      Close();
      host_ = std::move(other.host_);
      port_ = other.port_;
      socket_options_ = other.socket_options_;
      timeouts_ = other.timeouts_;
      sock_ = other.sock_;
      other.sock_ = kInvalidSocket;
    }
    return *this;
  }

  Client& SetTCPNodely(bool status) {
    socket_options_.tcp_nodelay = status;
    return *this;
  }

  Client& SetConnectionTimeout(std::chrono::seconds timeout) {
    timeouts_.connection_timeout = timeout;
    return *this;
  }

  Client& SetReadTimeout(std::chrono::seconds timeout) {
    timeouts_.read_timeout = timeout;
    return *this;
  }

  Client& SetWriteTimeout(std::chrono::seconds timeout) {
    timeouts_.write_timeout = timeout;
    return *this;
  }

  socket_types::Error connect() {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ == kInvalidSocket) {
      return socket_types::Error::Connection;
    }

    socket_types::ApplySocketOptions(sock_, socket_options_);

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) <= 0) {
      Close();
      return socket_types::Error::Connection;
    }

    if (::connect(sock_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
      Close();
      return socket_types::Error::Connection;
    }

    return socket_types::Error::Success;
  }

  std::unique_ptr<SocketStream> Stream() {
    if (sock_ == kInvalidSocket) return nullptr;
    return std::make_unique<SocketStream>(sock_, timeouts_);
  }

  void Close() {
    if (sock_ != kInvalidSocket) {
      socket_types::CloseSocket(sock_);
      sock_ = kInvalidSocket;
    }
  }

  [[nodiscard]] bool is_connected() const { return sock_ != kInvalidSocket; }

 private:
  int port_;
  std::string host_;
  socket_t sock_{kInvalidSocket};
  socket_types::TimeoutConfig timeouts_;
  socket_types::SocketOptions socket_options_;
};

}  // namespace camille

#endif
