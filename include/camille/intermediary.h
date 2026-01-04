#ifndef CAMILLE_INCLUDE_CAMILLE_INTERMEDIARY_H_
#define CAMILLE_INCLUDE_CAMILLE_INTERMEDIARY_H_

#include <expected>
#include <memory>
#include <vector>
#include <print>

#include "infra.h"
#include "socket_types.h"
#include "stream.h"
#include "server.h"

/**
 * @brief acts as an intermediary between the client and the socket backend
 */

namespace camille {
namespace intermediary {

inline infra::types::ParserData ParseBuffer(std::string_view buffer) {
  std::vector<std::string> helper;
  infra::types::ParserData parsed_data;

  return parsed_data;
}

inline std::expected<size_t, infra::NetworkError> SafeRead(
    std::unique_ptr<stream::SocketStream>& stream, std::vector<std::byte>& buffer) {
  auto result = stream->Read(buffer.data(), buffer.size());

  if (result < 0) {
    if (stream->GetError() == socket_types::Error::ConnectionTimeout) {
      return std::unexpected(infra::NetworkError::Timeout);
    }
    return std::unexpected(infra::NetworkError::ConnectionReset);
  }

  if (result == 0) {
    return std::unexpected(infra::NetworkError::RemoteClosed);
  }

  return static_cast<size_t>(result);
}

// TODO: make it prettier
inline void ProcessTransaction(std::unique_ptr<stream::SocketStream>& stream) {
  std::vector<std::byte> buffer(4096);
  auto bytes_read = SafeRead(stream, buffer);
  if (!bytes_read) {
    if (bytes_read.error() == infra::NetworkError::Timeout) {
      std::println(stderr, "Security Alert: Connection timed out (Potential Slowloris)");
    } else if (bytes_read.error() == infra::NetworkError::RemoteClosed) {
      std::println("Client closed connection gracefully (EOF)");
    } else if (bytes_read.error() == infra::NetworkError::ConnectionReset) {
      std::println(stderr, "Connection error: remote reset/abrupt close");
    } else {
      std::println(stderr, "Unknown network error while reading from socket");
    }
    return;
  }

  if (*bytes_read > 0) {
    std::string_view result{reinterpret_cast<const char*>(buffer.data()), *bytes_read};
    auto parsed_data = ParseBuffer(result);
    std::println("Received {} bytes: \n{}", *bytes_read, result);

    if (stream->Write(buffer.data(), *bytes_read) < 0) {
      std::println("Failed to write {} bytes to client", *bytes_read);
    }
  }
}

inline server::Server& Run(std::optional<server::Server>& server,
                           const std::string& client_ip,
                           int client_port) {
  server.emplace(client_ip, client_port);
  server->SetTCPNodelay(true)
      .SetReadTimeout(std::chrono::seconds(10))
      .SetWriteTimeout(std::chrono::seconds(30))
      .SetConnectionHandler([&](std::unique_ptr<camille::stream::SocketStream> stream) {
        int remote_port = 0;
        std::string remote_ip;
        stream->GetRemoteIpAndPort(remote_ip, remote_port);
        std::println("Client connected: {}:{}", remote_ip, remote_port);

        if (server->IsRunning()) {
          intermediary::ProcessTransaction(stream);
        } else {
          throw std::runtime_error("Server is not running.");
        }

        std::println("Client disconnected: {}:{}", remote_ip, remote_port);
      });
  if (!server->Listen()) {
    std::println("Failed to start Server, is the port taken?");
    throw std::runtime_error("Server error, try replacing the port.");
  }
  return *server;
}

};  // namespace intermediary
};  // namespace camille

#endif