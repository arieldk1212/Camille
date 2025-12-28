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
#include <stdexcept>
#include <string>

namespace camille {

namespace client {

static std::expected<size_t, infra::NetworkError> SafeRead(
    std::unique_ptr<camille::stream::SocketStream>& stream, std::span<std::byte> buffer) {
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

static void ProcessRequest(std::unique_ptr<camille::stream::SocketStream>& stream,
                           camille::server::Server& server) {
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

class BaseClient {
 public:
  virtual ~BaseClient() = default;

  virtual void Run(const std::string& base_client_ip, int base_client_port) = 0;
  virtual void AddMiddleware(const camille::middleware::BaseMiddleware& middleware) = 0;
};
};  // namespace client

class Camille : public client::BaseClient {
 public:
  Camille() = default;
  ~Camille() override { server_->Stop(); }

  Camille(const Camille&) = delete;
  Camille& operator=(const Camille&) = delete;

  Camille(Camille&&) = default;
  Camille& operator=(Camille&&) = default;

  void SetDebug(bool debug) { debug_ = debug; };
  void SetServerName(const std::string& server_name) { server_name_ = server_name; }
  void SetServerVersion(const std::string& server_version) { server_version_ = server_version; }

  void Run(const std::string& client_ip, int client_port) override {
    server_.emplace(client_ip, client_port);
    server_->SetTCPNodelay(true)
        .SetReadTimeout(std::chrono::seconds(10))
        .SetWriteTimeout(std::chrono::seconds(30))
        .SetConnectionHandler([this](std::unique_ptr<camille::stream::SocketStream> stream) {
          int remote_port = 0;
          std::string remote_ip;
          stream->GetRemoteIpAndPort(remote_ip, remote_port);
          std::println("Client connected: {}:{}", remote_ip, remote_port);

          if (server_->IsRunning()) {
            client::ProcessRequest(stream, *this->server_);
          } else {
            throw std::runtime_error("Server is not running.");
          }

          std::println("Client disconnected: {}:{}", remote_ip, remote_port);
        });
    if (!server_->Listen()) {
      std::println("Failed to start Server, is the port taken?");
      throw std::runtime_error("Server error, try replacing the port.");
    }
  }

  void AddMiddleware(const camille::middleware::BaseMiddleware& middleware) override {
    std::println("Middleware Added");
  }

 private:
  bool debug_{false};
  std::string server_name_;
  std::string server_version_;
  std::optional<camille::server::Server> server_;
};

};  // namespace camille

#endif
