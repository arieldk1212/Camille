#ifndef CAMILLE_INCLUDE_CAMILLE_SOCKET_TYPES_H_
#define CAMILLE_INCLUDE_CAMILLE_SOCKET_TYPES_H_

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
constexpr socket_t kInvalidSocket = INVALID_SOCKET;
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <cerrno>
using socket_t = int;
constexpr socket_t kInvalidSocket = -1;
#endif

#include <chrono>
#include <functional>

namespace camille {

namespace socket_types {

enum class Error : std::uint8_t {
  Success = 0,
  Connection,
  BindIPAddress,
  Read,
  Write,
  ExceedRedirectCount,
  Canceled,
  SSLConnection,
  SSLLoadingCerts,
  SSLServerVerification,
  UnsupportedMultipartBoundaryChars,
  Compression,
  ConnectionTimeout,
  Unknown
};

inline std::string ToString(Error error) {
  switch (error) {
    case Error::Success:
      return "Success";
    case Error::Connection:
      return "Connection failed";
    case Error::BindIPAddress:
      return "Failed to bind IP address";
    case Error::Read:
      return "Read error";
    case Error::Write:
      return "Write error";
    case Error::ConnectionTimeout:
      return "Connection timeout";
    case Error::Canceled:
      return "Canceled";
    default:
      return "Unknown error";
  }
}

struct SocketOptions {
  bool tcp_nodelay = false;
  bool ipv6_v6only = false;
  bool reuse_addr = true;
  bool reuse_port = true;
  int recv_buffer_size = 0;
  int send_buffer_size = 0;
};

constexpr int CONNECTION_TIMEOUT = 30;
constexpr int READ_TIMEOUT = 30;
constexpr int WRITE_TIMEOUT = 30;
constexpr int IDLE_INTERVAL = 30;

struct TimeoutConfig {
  std::chrono::seconds connection_timeout{CONNECTION_TIMEOUT};
  std::chrono::seconds read_timeout{READ_TIMEOUT};
  std::chrono::seconds write_timeout{WRITE_TIMEOUT};
  std::chrono::seconds idle_interval{IDLE_INTERVAL};
};

using SocketOptionsCallback = std::function<void(socket_t)>;

inline void CloseSocket(socket_t sock) {
  if (sock == kInvalidSocket) return;
#ifdef _WIN32
  closesocket(sock);
#else
  close(sock);
#endif
}

inline bool ApplySocketOptions(socket_t sock, const SocketOptions& opts) {
  if (sock == kInvalidSocket) {
    return false;
  }
  int yes = 1;

  if (opts.reuse_addr) {
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes),
                   sizeof(yes)) < 0) {
      return false;
    }
  }

#ifdef SO_REUSEPORT
  if (opts.reuse_port) {
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char*>(&yes),
                   sizeof(yes)) < 0) {
      return false;
    }
  }
#endif

  if (opts.tcp_nodelay) {
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&yes),
                   sizeof(yes)) < 0) {
      return false;
    }
  }

  if (opts.recv_buffer_size > 0) {
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&opts.recv_buffer_size),
               sizeof(opts.recv_buffer_size));
  }
  if (opts.send_buffer_size > 0) {
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&opts.send_buffer_size),
               sizeof(opts.send_buffer_size));
  }

  return true;
}

inline bool WaitUntilSocketReady(socket_t sock,
                                 std::chrono::milliseconds timeout,
                                 bool for_read,
                                 bool for_write) {
#ifdef _WIN32
  fd_set fds_read, fds_write, fds_err;
  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);

  if (for_read) FD_SET(sock, &fds_read);
  if (for_write) FD_SET(sock, &fds_write);
  FD_SET(sock, &fds_err);

  timeval tv;
  tv.tv_sec = static_cast<long>(timeout.count() / 1000);
  tv.tv_usec = static_cast<long>((timeout.count() % 1000) * 1000);

  auto ret = select(static_cast<int>(sock + 1), for_read ? &fds_read : nullptr,
                    for_write ? &fds_write : nullptr, &fds_err, &tv);
  return ret > 0 && !FD_ISSET(sock, &fds_err);
#else
  pollfd pfd{};
  pfd.fd = sock;
  pfd.events = 0;
  if (for_read) pfd.events |= POLLIN;
  if (for_write) pfd.events |= POLLOUT;

  auto ret = poll(&pfd, 1, static_cast<int>(timeout.count()));
  return ret > 0 && !(pfd.revents & (POLLERR | POLLHUP | POLLNVAL));
#endif
}

};  // namespace socket_types
};  // namespace camille

#endif
