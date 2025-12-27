#ifndef CAMILLE_INCLUDE_CAMILLE_CLIENT_H_
#define CAMILLE_INCLUDE_CAMILLE_CLIENT_H_

#include "server.h"

#include <stdexcept>
#include <string>

namespace camille {

class BaseClient {
 public:
  virtual ~BaseClient() = default;

  virtual void Run(const std::string& base_client_ip, int base_client_port) = 0;
};

class CamilleClient : public BaseClient {
 public:
  CamilleClient() = default;
  ~CamilleClient() override {
    if (server_) server_->Stop();
  }

  CamilleClient(const CamilleClient&) = delete;
  CamilleClient& operator=(const CamilleClient&) = delete;

  CamilleClient(CamilleClient&&) = default;
  CamilleClient& operator=(CamilleClient&&) = default;

  void Run(const std::string& client_ip, int client_port) override {
    server_.emplace(client_ip, client_port);
    server_->SetTCPNodelay(true)
        .SetReadTimeout(std::chrono::seconds(30))
        .SetWriteTimeout(std::chrono::seconds(30))
        .SetConnectionHandler([](std::unique_ptr<camille::SocketStream> stream) {
          int client_port = 0;
          std::string client_ip;
          stream->GetRemoteIpAndPort(client_ip, client_port);
          std::println("Client connected: {}:{}", client_ip, client_port);

          char buffer[1024];
          auto bytes_read = stream->Read(buffer, sizeof(buffer) - 1);
          if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::println("Received: {}", buffer);
            stream->Write(buffer, static_cast<size_t>(bytes_read));
          }

          std::println("Client disconnected: {}:{}", client_ip, client_port);
        });
    if (!server_->Listen()) {
      std::println("Failed to start Server, is the port taken?");
      throw std::runtime_error("Server error, try replacing the port.");
    }
  }

 private:
  std::optional<camille::Server> server_;
};

};  // namespace camille

#endif

/**
#include <expected>
#include <vector>
#include <span>
#include <string_view>
#include <print>
#include <chrono>

enum class NetworkError { Timeout, ConnectionReset, QuotaExceeded };

// A modern, safe wrapper for the read operation
std::expected<size_t, NetworkError> safe_read(auto& stream, std::span<std::byte> buffer) {
    // 1. Set a deadline for the read operation (Slowloris protection)
    stream.set_read_timeout(std::chrono::seconds(5));

    auto result = stream.Read(buffer.data(), buffer.size());

    if (result < 0) return std::unexpected(NetworkError::ConnectionReset);
    if (result == 0) return std::unexpected(NetworkError::Timeout); // Treat 0 as timeout or close

    return static_cast<size_t>(result);
}

void process_request(auto& stream) {
    std::vector<std::byte> buffer(1024);

    // Attempt the read and handle the result/error elegantly
    auto bytes_read = safe_read(stream, buffer);

    if (!bytes_read) {
        switch (bytes_read.error()) {
            case NetworkError::Timeout:
                std::println(stderr, "Slowloris detected or connection idle. Dropping.");
                stream.Close();
                break;
            default:
                std::println(stderr, "Network error occurred.");
        }
        return;
    }

    // Success: Process only the slice of memory that was actually filled
    std::span<std::byte> data_slice{buffer.data(), *bytes_read};

    // Print safely using string_view (no null terminator needed)
    std::string_view message{reinterpret_cast<char*>(data_slice.data()), data_slice.size()};
    std::println("Received: {}", message);

    stream.Write(data_slice.data(), data_slice.size());
}

 */