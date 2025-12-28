#ifndef CAMILLE_INCLUDE_CAMILLE_CLIENT_H_
#define CAMILLE_INCLUDE_CAMILLE_CLIENT_H_

#include "camille/stream.h"
#include "middleware.h"
#include "server.h"
#include "infra.h"

#include <expected>
#include <print>
#include <span>
#include <cstddef>
#include <string>

namespace camille {

class BaseClient {
 public:
  virtual ~BaseClient() = default;

  virtual void Run(const std::string& base_client_ip, int base_client_port) = 0;
  virtual void AddMiddleware(const BaseMiddleware& middleware) = 0;
};

class CamilleClient : public BaseClient {
 public:
  CamilleClient() = default;
  ~CamilleClient() override { server_->Stop(); }

  CamilleClient(const CamilleClient&) = delete;
  CamilleClient& operator=(const CamilleClient&) = delete;

  CamilleClient(CamilleClient&&) = default;
  CamilleClient& operator=(CamilleClient&&) = default;

  void Run(const std::string& client_ip, int client_port) override {
    server_.emplace(client_ip, client_port);
    server_->SetTCPNodelay(true)
        .SetReadTimeout(std::chrono::seconds(10))
        .SetWriteTimeout(std::chrono::seconds(30))
        .SetConnectionHandler([this](std::unique_ptr<camille::SocketStream> stream) {
          int remote_port = 0;
          std::string remote_ip;
          stream->GetRemoteIpAndPort(remote_ip, remote_port);
          std::println("Client connected: {}:{}", remote_ip, remote_port);

          if (server_->IsRunning()) {
            ProcessRequest(stream, *this->server_);
          }

          std::println("Client disconnected: {}:{}", remote_ip, remote_port);
        });
    if (!server_->Listen()) {
      std::println("Failed to start Server, is the port taken?");
      throw std::runtime_error("Server error, try replacing the port.");
    }
  }

  void AddMiddleware(const BaseMiddleware& middleware) override {
    std::println("Middleware Added");
  }

 private:
  static std::expected<size_t, infra::NetworkError> SafeRead(
      std::unique_ptr<camille::SocketStream>& stream, std::span<std::byte> buffer) {
    auto result = stream->Read(buffer.data(), buffer.size());

    if (result < 0) {
      if (stream->GetError() == socket_types::Error::Read) {
        return std::unexpected(infra::NetworkError::Timeout);
      }
      return std::unexpected(infra::NetworkError::ConnectionReset);
    }

    if (result == 0) {
      return std::unexpected(infra::NetworkError::ConnectionReset);
    }

    return static_cast<size_t>(result);
  }

  static void ProcessRequest(std::unique_ptr<camille::SocketStream>& stream,
                             camille::Server& server) {
    std::vector<std::byte> buffer(4096);

    auto bytes_read = SafeRead(stream, buffer);

    if (!bytes_read) {
      if (bytes_read.error() == infra::NetworkError::Timeout) {
        std::println(stderr, "Security Alert: Connection timed out (Potential Slowloris)");
      }
      return;
    }

    if (*bytes_read > 0) {
      std::string_view result{reinterpret_cast<const char*>(buffer.data()), *bytes_read};
      std::println("Received {} bytes: {}", *bytes_read, result);

      stream->Write(buffer.data(), *bytes_read);
    }
  }

  std::optional<camille::Server> server_;
};

};  // namespace camille

#endif
