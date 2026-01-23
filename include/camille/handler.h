#ifndef CAMILLE_INCLUDE_CAMILLE_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_HANDLER_H_

#include <optional>
#include "camille/logging.h"
#include "request.h"
#include "response.h"
#include "parser.h"

namespace camille {
namespace handler {

class RequestHandler {
 public:
  RequestHandler() = default;

  std::optional<request::Request> Parse(std::string_view data) {
    auto req = parser_.Parse<request::Request>(data);
    if (!parser_) {
      CAMILLE_ERROR("Request parsing error: {} | ec: {}", parser_.GetErrorString(),
                    parser_.GetErrorCode());
      return std::nullopt;
    }
    if (!req) {
      CAMILLE_ERROR("Parser error, no value found");
      return std::nullopt;
    }
    return req.value();
  }

 private:
  parser::Parser parser_;
};

class ResponseHandler {
 public:
  ResponseHandler() = default;

  std::optional<response::Response> Parse(std::string_view data) {
    auto res = parser_.Parse<response::Response>(data);
    if (!parser_) {
      CAMILLE_ERROR("Response parsing error: {} | ec: {}", parser_.GetErrorString(),
                    parser_.GetErrorCode());
      return std::nullopt;
    }
    if (!res) {
      CAMILLE_ERROR("Parser error, no value found");
      return std::nullopt;
    }
    return res.value();
  }

 private:
  parser::Parser parser_;
};

};  // namespace handler
};  // namespace camille.

#endif