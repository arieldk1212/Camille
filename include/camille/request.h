#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_H_

#include "camille/types.h"
#include "infra.h"

#include <string>

// TODO: implement: Zero-Copy Parsing
// TODO: add main camille headers
/**
 * @example
    Client connected: 127.0.0.1:0
    Received 432 bytes:
      GET / HTTP/1.1
      Host: 0.0.0.0:8085
      Connection: keep-alive
      Cache-Control: max-age=0
      Upgrade-Insecure-Requests: 1
      User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like
      Gecko) Chrome/143.0.0.0 Safari/537.36 Accept:
      text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*//*;q=0.8
      Sec-GPC: 1
      Accept-Language: en-US,en;q=0.8
      Accept-Encoding: gzip, deflate
 */

namespace camille {

namespace request {

class Request {
 public:
  explicit Request(types::aio::AsioIOSocket&& socket)
      : socket_(std::move(socket)) {}
  ~Request() = default;

  bool AddHeader(const std::string& header_name, const std::string& header_value);

 private:
  types::aio::AsioIOSocket socket_;
  types::aio::AsioIOStreamBuffer stream_buffer_;
  std::string path_;
  std::string host_;
  std::uint16_t port_;
  std::string body_;
  size_t request_size_;
  bool has_auth_{false};
  infra::Methods method_;
  infra::headers::RequestHeaders headers_;
};

};  // namespace request
};  // namespace camille

#endif