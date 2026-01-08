#ifndef CAMILLE_INCLUDE_CAMILLE_NETWORK_H_
#define CAMILLE_INCLUDE_CAMILLE_NETWORK_H_

#include "types.h"
#include "logging.h"

#include "asio/read_until.hpp"
#include "asio/write.hpp"

#include <cstddef>
#include <memory>

namespace camille {
namespace network {

class Session : public std::enable_shared_from_this<Session> {
  /**
   * @todo to add ssl we need do_handshake(), pass it to start()
   * @brief check out boost implementation for server.
   */
 public:
  explicit Session(types::aio::AsioIOSocket socket)
      : socket_(types::camille::CamilleShared<types::aio::AsioIOSocket>(&socket)) {}

  void Start() { DoRead(); }

 private:
  void DoRead() {
    auto self(shared_from_this());
    asio::async_read_until(
        *socket_, stream_buffer_, "\n",
        [this, self](const std::error_code& error_code, size_t bytes) mutable -> void {
          if (!error_code) {
            auto buffers = stream_buffer_.data();
            std::string buffer_data(
                asio::buffers_begin(buffers),
                asio::buffers_end(buffers) + static_cast<std::ptrdiff_t>(bytes));
            DoWrite(bytes);
          } else {
            CAMILLE_ERROR(error_code.message());
          }
        });
  }

  void DoWrite(size_t bytes_to_write) {
    auto self(shared_from_this());
    asio::async_write(
        *socket_, stream_buffer_, asio::transfer_exactly(bytes_to_write),
        [this, self, bytes_to_write](const std::error_code& error_code, size_t) mutable -> void {
          if (!error_code) {
            //   stream_buffer_.consume(bytes_to_write);
            DoRead();
          } else {
            CAMILLE_ERROR(error_code.message());
          }
        });
  }

  types::aio::AsioIOStreamBuffer stream_buffer_;
  types::camille::CamilleShared<types::aio::AsioIOSocket> socket_;
};

};  // namespace network
};  // namespace camille

/**
* class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(types::aio::AsioIOSocket socket)
        : socket_(std::move(socket)) {}

    void Start() { ReadHeaders(); }

private:
    void ReadHeaders() {
        auto self(shared_from_this());
        // HTTP headers end with a double CRLF
        asio::async_read_until(socket_, stream_buffer_, "\r\n\r\n",
            [this, self](const std::error_code& ec, size_t bytes) {
                if (!ec) {
                    // Extract headers from the buffer
                    auto buffers = stream_buffer_.data();
                    std::string header_data(
                        asio::buffers_begin(buffers),
                        asio::buffers_begin(buffers) + static_cast<std::ptrdiff_t>(bytes)
                    );

                    // 1. Consume the headers from the buffer
                    stream_buffer_.consume(bytes);

                    // 2. Parse Content-Length to see if there is a body
                    size_t content_length = ParseContentLength(header_data);

                    if (content_length > 0) {
                        ReadBody(content_length);
                    } else {
                        HandleRequest(header_data, "");
                    }
                }
            });
    }

    void ReadBody(size_t length) {
        auto self(shared_from_this());

        // Some data might already be in the streambuf!
        // We only need to read the remainder.
        size_t remaining = (length > stream_buffer_.size()) ? (length - stream_buffer_.size()) : 0;

        asio::async_read(socket_, stream_buffer_, asio::transfer_exactly(remaining),
            [this, self, length](const std::error_code& ec, size_t bytes) {
                if (!ec) {
                    auto buffers = stream_buffer_.data();
                    std::string body_data(
                        asio::buffers_begin(buffers),
                        asio::buffers_begin(buffers) + static_cast<std::ptrdiff_t>(length)
                    );

                    stream_buffer_.consume(length);
                    HandleRequest("Headers already parsed", body_data);
                }
            });
    }

    void HandleRequest(std::string_view headers, std::string_view body) {
        // Your framework logic: Routing, Controllers, etc.
        std::println("Request Handled. Sending 200 OK.");
        SendResponse("HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK");
    }

    void SendResponse(std::string response) {
        auto self(shared_from_this());
        auto response_ptr = std::make_shared<std::string>(std::move(response));
        asio::async_write(socket_, asio::buffer(*response_ptr),
            [this, self, response_ptr](std::error_code ec, size_t) {
                if (!ec) {
                    // Keep-alive logic: read the next request
                    ReadHeaders();
                }
            });
    }

    // Helper to extract content length from header string
    size_t ParseContentLength(std::string_view headers) {
        // Implement regex or string search for "Content-Length: "
        return 0;
    }

    types::aio::AsioIOSocket socket_;
    types::aio::AsioIOStreamBuffer stream_buffer_;
};
 */

/*
namespace camille::network {

class Session : public std::enable_shared_from_this<Session> {
public:
   explicit Session(types::aio::AsioIOSocket socket)
       : socket_(std::move(socket)) {}

   void Start() {
       DoRead();
   }

private:
   void DoRead() {
       auto self(shared_from_this());

       // We look for the HTTP header terminator
       asio::async_read_until(socket_, stream_buffer_, "\r\n\r\n",
           [this, self](const std::error_code& ec, size_t bytes_transferred) {
               if (!ec) {
                   ProcessBuffer(bytes_transferred);
               } else if (ec != asio::error::eof) {
                   std::println("Read Error: {}", ec.message());
                   // Socket will close when 'self' goes out of scope
               }
           });
   }

   void ProcessBuffer(size_t bytes_to_process) {
       // 1. Get access to the internal buffer data
       auto buffers = stream_buffer_.data();

       // 2. Extract exactly the amount asio found (the headers)
       // Fixed narrowing: start + bytes
       std::string raw_data(
           asio::buffers_begin(buffers),
           asio::buffers_begin(buffers) + static_cast<std::ptrdiff_t>(bytes_to_process)
       );

       // --- AT THIS POINT: raw_data contains your headers ---
       std::println("Read {} bytes of header data.", bytes_to_process);

       // 3. Remove what we processed from the stream_buffer
       // This is critical. It shifts any 'extra' data (the body or next request)
       // to the beginning of the buffer.
       stream_buffer_.consume(bytes_to_process);

       // 4. For now, let's just loop back to wait for more data
       // In a real framework, this is where you'd decide to ReadBody or SendResponse
       DoRead();
   }

private:
   types::aio::AsioIOSocket socket_;
   types::aio::AsioIOStreamBuffer stream_buffer_;
};

} // namespace camille::network
 */

#endif