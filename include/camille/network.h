#ifndef CAMILLE_INCLUDE_CAMILLE_NETWORK_H_
#define CAMILLE_INCLUDE_CAMILLE_NETWORK_H_

#include "asio/read_until.hpp"
#include "asio/write.hpp"
#include "types.h"

#include <memory>

namespace camille {
namespace network {

class SessionRequest {
 public:
  explicit SessionRequest(types::aio::AsioIOSocket&& socket)
      : socket_(std::move(socket)) {}

 private:
  types::aio::AsioIOSocket socket_;
  types::aio::AsioIOStreamBuffer stream_buffer_;
};

class Session : public std::enable_shared_from_this<Session> {
  /**
   * @todo to add ssl we need do_handshake(), pass it to start()
   * @brief check out boost implementation for server.
   */
 public:
  explicit Session(types::aio::AsioIOSocket socket)
      : socket_(std::move(socket)) {}

  void start() { do_read(); }

 private:
  void do_read() {
    auto self(shared_from_this());
    asio::async_read_until(socket_, stream_buffer_, "\n",
                           [this, self](const std::error_code& error_code, size_t bytes) {
                             if (!error_code) {
                               std::println("Recieved {} Bytes", bytes);
                               do_write();
                             }
                           });
  }

  void do_write() {
    auto self(shared_from_this());
    asio::async_write(socket_, stream_buffer_,
                      [this, self](const std::error_code& error_code, std::size_t /*length*/) {
                        if (!error_code) {
                          do_read();
                        }
                      });
  }

 private:
  types::aio::AsioIOSocket socket_;
  types::aio::AsioIOStreamBuffer stream_buffer_;
};

};  // namespace network
};  // namespace camille

#endif