#ifndef CAMILLE_INCLUDE_CAMILLE_STREAM_H_
#define CAMILLE_INCLUDE_CAMILLE_STREAM_H_

#include "socket_types.h"

#include <cstring>
#include <string>
#include <chrono>

namespace camille {

class Stream {
 public:
  virtual ~Stream() = default;

  [[nodiscard]] virtual bool IsReadable() const = 0;
  [[nodiscard]] virtual bool IsWriteable() const = 0;
  [[nodiscard]] virtual bool WaitReadable(std::chrono::milliseconds timeout) const = 0;
  [[nodiscard]] virtual bool WaitWriteable(std::chrono::milliseconds timeout) const = 0;

  virtual ssize_t Read(char* ptr, size_t size) = 0;
  virtual ssize_t Write(const char* ptr, size_t size) = 0;

  [[nodiscard]] virtual socket_t Socket() const = 0;
  [[nodiscard]] virtual socket_types::Error GetError() const = 0;

  virtual void GetLocalIpAndPort(std::string& ip_address, int port) const = 0;
  virtual void GetRemoteIpAndPort(std::string& ip_address, int port) const = 0;
};

class SocketStream : public Stream {
 public:
  explicit SocketStream(socket_t sock, const socket_types::TimeoutConfig& timeouts = {})
      : sock_(sock),
        read_timeout_(timeouts.read_timeout),
        write_timeout_(timeouts.write_timeout) {}

  ~SocketStream() override = default;

  SocketStream(const SocketStream&) = delete;
  SocketStream& operator=(const SocketStream&) = delete;
  SocketStream(SocketStream&& other) noexcept
      : sock_(other.sock_) {
    other.sock_ = kInvalidSocket;
  }
  SocketStream& operator=(SocketStream&& other) noexcept {
    if (this != &other) {
      sock_ = other.sock_;
      other.sock_ = kInvalidSocket;
    }
    return *this;
  }

  [[nodiscard]] bool IsReadable() const override {
    return socket_types::WaitUntilSocketReady(sock_, std::chrono::milliseconds(0), true, false);
  }

  [[nodiscard]] bool IsWriteable() const override {
    return socket_types::WaitUntilSocketReady(sock_, std::chrono::milliseconds(0), false, true);
  }

  [[nodiscard]] bool WaitReadable(std::chrono::milliseconds timeout) const override {
    return socket_types::WaitUntilSocketReady(sock_, timeout, true, false);
  }

  [[nodiscard]] bool WaitWriteable(std::chrono::milliseconds timeout) const override {
    return socket_types::WaitUntilSocketReady(sock_, timeout, false, true);
  }

  ssize_t Write(const char* ptr) { return Write(ptr, strlen(ptr)); }
  ssize_t write(const std::string& s) { return Write(s.data(), s.size()); }

  [[nodiscard]] socket_types::Error GetError() const override { return error_; };

  ssize_t Read(char* ptr, size_t size) override {
    if (!WaitReadable(std::chrono::duration_cast<std::chrono::milliseconds>(read_timeout_))) {
      error_ = socket_types::Error::Read;
      return -1;
    }

#ifdef _WIN32
    auto ret = recv(sock_, ptr, static_cast<int>(size), 0);
#else
    auto ret = recv(sock_, ptr, size, 0);
#endif

    if (ret < 0) {
      error_ = socket_types::Error::Read;
    }
    return ret;
  }

  ssize_t Write(const char* ptr, size_t size) override {
    if (!WaitWriteable(std::chrono::duration_cast<std::chrono::milliseconds>(write_timeout_))) {
      error_ = socket_types::Error::Write;
      return -1;
    }

    size_t total_written = 0;
    while (total_written < size) {
#ifdef _WIN32
      auto ret = send(sock_, ptr + total_written, static_cast<int>(size - total_written), 0);
#else
      auto ret = send(sock_, ptr + total_written, size - total_written, 0);
#endif
      if (ret < 0) {
        error_ = socket_types::Error::Write;
        return -1;
      }
      total_written += static_cast<size_t>(ret);
    }
    return static_cast<ssize_t>(total_written);
  }

  void GetRemoteIpAndPort(std::string& ip_address, int port) const override {
    struct sockaddr_storage addr{};
    socklen_t len = sizeof(addr);
    if (getpeername(sock_, reinterpret_cast<struct sockaddr*>(&addr), &len) == 0) {
      ExtractIpAndPort(addr, ip_address, port);
    }
  }

  void GetLocalIpAndPort(std::string& ip_address, int port) const override {
    struct sockaddr_storage addr{};
    socklen_t len = sizeof(addr);
    if (getsockname(sock_, reinterpret_cast<struct sockaddr*>(&addr), &len) == 0) {
      ExtractIpAndPort(addr, ip_address, port);
    }
  }

  [[nodiscard]] socket_t Socket() const override { return sock_; }

  void SetReadTimeout(std::chrono::seconds timeout) { read_timeout_ = timeout; }
  void SetWriteTimeout(std::chrono::seconds timeout) { write_timeout_ = timeout; }

 private:
  static void ExtractIpAndPort(const sockaddr_storage& addr, std::string& ip_address, int& port) {
    char buf[INET6_ADDRSTRLEN];
    if (addr.ss_family == AF_INET) {
      const auto* sin = reinterpret_cast<const sockaddr_in*>(&addr);
      inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
      ip_address = buf;
      port = ntohs(sin->sin_port);
    } else if (addr.ss_family == AF_INET6) {
      const auto* sin6 = reinterpret_cast<const sockaddr_in6*>(&addr);
      inet_ntop(AF_INET6, &sin6->sin6_addr, buf, sizeof(buf));
      ip_address = buf;
      port = ntohs(sin6->sin6_port);
    }
  }

  socket_t sock_;
  socket_types::Error error_ = socket_types::Error::Success;
  std::chrono::seconds read_timeout_{socket_types::READ_TIMEOUT};
  std::chrono::seconds write_timeout_{socket_types::WRITE_TIMEOUT};
};

}  // namespace camille

#endif
