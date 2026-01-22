#ifndef CAMILLE_INCLUDE_CAMILLE_SERVER_H_
#define CAMILLE_INCLUDE_CAMILLE_SERVER_H_

#include "network.h"
#include "pool.h"
#include "logging.h"

#include "asio/ip/address.hpp"

#include <system_error>
#include <thread>

namespace camille {
namespace server {

class Server {
 public:
  Server(const std::string& host,
         std::uint16_t port,
         concepts::UnsignedIntegral auto pool_size = std::thread::hardware_concurrency())
      : io_context_pool_(pool_size == 0 ? 1 : pool_size),
        acceptor_(io_context_pool_.GetIOContext(),
                  asio::ip::tcp::endpoint(asio::ip::make_address(host), port)) {
    if (pool_size == 0) {
      CAMILLE_CRITICAL("Server pool_size initialized with 0, Defaulting to 1");
    }
    StartAccept();
  }
  ~Server() {
    io_context_pool_.Stop();
    if (acceptor_.is_open()) {
      std::error_code error_code;
      acceptor_.close(error_code);
    }
  }

  explicit operator bool() const { return acceptor_.is_open(); }

  void SetState(bool state) { state_ = state; }

  void Run(std::function<void()> callback) {
    io_context_pool_.Run();
    callback();
    io_context_pool_.Wait();
  }

 private:
  void StartAccept() {
    auto& client_ctx = io_context_pool_.GetIOContext();
    auto new_socket = std::make_shared<types::aio::AsioIOSocket>(client_ctx);

    acceptor_.async_accept(*new_socket, [this, new_socket](const std::error_code& error_code) {
      if (!error_code) {
        std::make_shared<network::Session>(new_socket, state_)->Start();
      } else {
        CAMILLE_CRITICAL("Async Accept Error, {}", error_code.message());
      }

      if (acceptor_.is_open()) {
        StartAccept();
        CAMILLE_DEBUG("Acceptor is open for connetions");
      } else {
        CAMILLE_ERROR("Server error, acceptor is closed");
      }
    });
  }

 private:
  bool state_{false};
  pool::ContextPool io_context_pool_;
  types::aio::AsioIOAcceptor acceptor_;
};

};  // namespace server
};  // namespace camille

#endif