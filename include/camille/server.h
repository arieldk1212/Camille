#ifndef CAMILLE_INCLUDE_CAMILLE_SERVER_H_
#define CAMILLE_INCLUDE_CAMILLE_SERVER_H_

#include "asio/ip/address.hpp"
#include "camille/types.h"
#include "network.h"
#include "pool.h"

#include <system_error>
#include <thread>

namespace camille {
namespace server {

class Server {
 public:
  Server(const std::string& host,
         std::uint16_t port,
         concepts::UnsignedIntegral auto pool_size = std::thread::hardware_concurrency())
      : io_context_pool_(pool_size),
        acceptor_(io_context_pool_.GetIOContext(),
                  asio::ip::tcp::endpoint(asio::ip::make_address(host), port)) {
    StartAccept();
  }

  bool operator()() const { return acceptor_.is_open(); }

  void Run() { io_context_pool_.Run(); }

 private:
  void StartAccept() {
    auto& client_ctx = io_context_pool_.GetIOContext();
    auto new_socket = std::make_shared<types::aio::AsioIOSocket>(client_ctx);

    acceptor_.async_accept(*new_socket, [this, new_socket](const std::error_code& error_code) {
      if (!error_code) {
        std::make_shared<network::Session>(std::move(*new_socket))->Start();
      }
      StartAccept();
    });
  }

 private:
  pool::ContextPool io_context_pool_;
  types::aio::AsioIOAcceptor acceptor_;
};

};  // namespace server
};  // namespace camille

#endif