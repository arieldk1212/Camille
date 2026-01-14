#ifndef CAMILLE_INCLUDE_CAMILLE_NETWORK_H_
#define CAMILLE_INCLUDE_CAMILLE_NETWORK_H_

#include "asio/basic_streambuf.hpp"
#include "asio/streambuf.hpp"
#include "types.h"
#include "logging.h"
#include "handler.h"

#include "asio/read_until.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"

#include <cstddef>
#include <memory>

namespace camille {
namespace network {

class Session : public std::enable_shared_from_this<Session> {
  /**
   * @todo to add ssl we need do_handshake(), pass it to start()
   * @todo add timeout with async_wait.
   * @brief check out boost implementation for server.
   */
 public:
  explicit Session(types::camille::CamilleShared<types::aio::AsioIOSocket> socket, bool state)
      : socket_(std::move(socket)),
        state_(state) {}

  void Start() { DoRead(); }

  bool GetState() const { return state_; }

 private:
  class ReadHandler {
   public:
    explicit ReadHandler(types::camille::CamilleShared<Session> ptr)
        : self(std::move(ptr)) {}

    void operator()(const std::error_code& error_code, std::size_t bytes) const {
      if (!error_code) {
        asio::streambuf::const_buffers_type buffer = self->stream_buffer_.data();

        std::string data(
            asio::buffers_begin(buffer),
            std::next(asio::buffers_begin(buffer), static_cast<std::ptrdiff_t>(bytes)));

        auto res = handler::Handler::Process(data);
        std::cout << res << " HERE!!! \n";

        // if (self->GetState()) {
        std::println("{}", data);
        // }

        self->stream_buffer_.consume(bytes);
        self->DoWrite(bytes);
      } else if (error_code == asio::error::eof) {
        CAMILLE_WARNING("Session ended");
      }
    }
    types::camille::CamilleShared<Session> self;
  };

  class WriteHandler {
   public:
    WriteHandler(types::camille::CamilleShared<Session> ptr, size_t consume)
        : self(std::move(ptr)),
          to_consume(consume) {}

    void operator()(const std::error_code& error_code, std::size_t) const {
      if (!error_code) {
        self->stream_buffer_.consume(to_consume);
        self->DoRead();
      } else if (error_code == asio::error::eof || error_code == asio::error::connection_reset ||
                 error_code == asio::error::broken_pipe) {
        CAMILLE_WARNING("Session ended");
      } else {
        CAMILLE_ERROR("Unexpected Session Error: {}", error_code.message());
      }
    }

    size_t to_consume;
    types::camille::CamilleShared<Session> self;
  };

  void DoRead() {
    asio::async_read_until(*socket_, stream_buffer_, "\r\n\r\n", ReadHandler{shared_from_this()});
    // asio::async_read(*socket_, stream_buffer_, ReadHandler{shared_from_this()});
  }

  void DoWrite(std::size_t bytes_to_write) {
    asio::async_write(*socket_, stream_buffer_, WriteHandler(shared_from_this(), bytes_to_write));
  }

  /**
   * @todo implement it with chorno for timeouts with condv.
   */
  void DoWait();

  bool state_{false};
  types::aio::AsioIOStreamBuffer stream_buffer_;
  types::camille::CamilleShared<types::aio::AsioIOSocket> socket_;
};

};  // namespace network
};  // namespace camille

#endif