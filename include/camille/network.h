#ifndef CAMILLE_INCLUDE_CAMILLE_NETWORK_H_
#define CAMILLE_INCLUDE_CAMILLE_NETWORK_H_

#include "error.h"
#include "types.h"
#include "logging.h"
#include "handler.h"

#include "asio/basic_streambuf.hpp"
#include "asio/streambuf.hpp"
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
    explicit ReadHandler(types::camille::CamilleShared<Session> ptr,
                         handler::RequestHandler& request_handler)
        : self(std::move(ptr)),
          self_request_handler(request_handler) {}

    void operator()(const std::error_code& error_code, size_t bytes) {
      if (!error_code) {
        asio::streambuf::const_buffers_type buffer = self->stream_buffer_.data();

        std::string_view data(static_cast<const char*>(buffer.data()),
                              static_cast<std::ptrdiff_t>(bytes));

        // std::string data(
        //     asio::buffers_begin(buffer),
        //     std::next(asio::buffers_begin(buffer), static_cast<std::ptrdiff_t>(bytes)));

        // auto result = self_request_handler.Parse(data);
        // if (result.has_value()) {
        //   self_request_handler.PrintRequest();

        // } else if (result.error() == error::Errors::kPartialMessage) {
        //   auto result = self_request_handler.Parse(data, true);
        //   if (result.has_value()) {
        //     self_request_handler.PrintRequest();
        //   }
        // } else {
        //   CAMILLE_ERROR("Parser Error: {}", static_cast<std::uint8_t>(result.error()));
        // }

        auto result = self_request_handler.Parse(data);
        if (!result) {
          if (result.error() == error::Errors::kPartialMessage) {
            self->DoRead();
            return;
          }
          CAMILLE_ERROR("Parser Error: {}", static_cast<std::uint8_t>(result.error()));
        }

        self_request_handler.PrintRequest();
        self->stream_buffer_.consume(bytes);
        self->DoWrite(bytes);

      } else if (error_code == asio::error::eof) {
        CAMILLE_WARNING("Session ended");
      }
    }

    types::camille::CamilleShared<Session> self;
    handler::RequestHandler self_request_handler;
  };

  class WriteHandler {
   public:
    WriteHandler(types::camille::CamilleShared<Session> ptr,
                 size_t consume,
                 handler::ResponseHandler& response_handler)
        : self(std::move(ptr)),
          to_consume(consume),
          self_response_handler(response_handler) {}

    void operator()(const std::error_code& error_code, size_t) const {
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
    handler::ResponseHandler self_response_handler;
  };

  void DoRead() {
    asio::async_read_until(*socket_, stream_buffer_, "\r\n\r\n",
                           ReadHandler{shared_from_this(), request_handler_});
    // asio::async_read(*socket_, stream_buffer_, ReadHandler{shared_from_this(),
    // request_handler_});
  }

  void DoWrite(std::size_t bytes_to_write) {
    asio::async_write(*socket_, stream_buffer_,
                      WriteHandler(shared_from_this(), bytes_to_write, response_handler_));
  }

  /**
   * @todo implement it with chorno for timeouts with condv.
   */
  void DoWait();

  bool state_{false};
  handler::RequestHandler request_handler_;
  handler::ResponseHandler response_handler_;
  types::aio::AsioIOStreamBuffer stream_buffer_;
  types::camille::CamilleShared<types::aio::AsioIOSocket> socket_;
};

};  // namespace network
};  // namespace camille

#endif