#ifndef CAMILLE_INCLUDE_CAMILLE_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_HANDLER_H_

#include "camille/logging.h"
#include "request.h"
#include "response.h"
#include "parser.h"

namespace camille {
namespace handler {

/**
* @example
  GET / HTTP/1.1
  Host: 127.0.0.1:8085
  Connection: keep-alive
  sec-ch-ua: "Brave";v="143", "Chromium";v="143", "Not A(Brand";v="24"
  sec-ch-ua-mobile: ?0
  sec-ch-ua-platform: "macOS"
  Upgrade-Insecure-Requests: 1
  User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like
Gecko) Chrome/143.0.0.0 Safari/537.36 Accept:
  text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*//*;q=0.8
    Sec-GPC: 1
    Accept-Language: en-US,en;q=0.5
    Sec-Fetch-Site: none
    Sec-Fetch-Mode: navigate
    Sec-Fetch-User: ?1
    Sec-Fetch-Dest: document
    Accept-Encoding: gzip, deflate, br, zstd
 */

class Handler {
 public:
  virtual ~Handler() = default;
};

class RequestHandler : public Handler {
 public:
  RequestHandler() = default;

  request::Request Parse(std::string_view data) {
    parser_.Parse(data, request_);
    if (!parser_) {
      CAMILLE_ERROR("request parsing error");
    }
    return request_;
  }

 private:
  parser::Parser parser_;
  request::Request request_;
};

class ResponseHandler : public Handler {
 public:
  ResponseHandler() = default;

  response::Response Parse(std::string_view data) {
    if (!parser_) {
      CAMILLE_ERROR("response parsing error");
    }
    parser_.Parse(data, response_);
    return response_;
  }

 private:
  parser::Parser parser_;
  response::Response response_;
};

};  // namespace handler
};  // namespace camille.

#endif