#ifndef CAMILLE_INCLUDE_CAMILLE_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_HANDLER_H_

#include <optional>

#include "error.h"
#include "logging.h"
#include "request.h"
#include "response.h"
#include "parser.h"

namespace camille {
namespace handler {

class RequestHandler {
 public:
  RequestHandler() = default;

  std::pair<std::optional<request::Request>, std::optional<error::Errors>> Parse(
      std::string_view data) {
    auto req = parser_.Parse<request::Request>(data);

    if (!parser_ && req.error() == error::Errors::kPartialMessage) {
      return {std::nullopt, req.error()};
    }

    if (req.has_value() && parser_.GetErrorCode() == error::Errors::kDefault) {
      return {req.value(), error::Errors::kDefault};
    }

    CAMILLE_ERROR("Request parsing error: {}", static_cast<std::uint8_t>(req.error()));
    return {std::nullopt, req.error()};
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
      CAMILLE_ERROR("Response parsing error: {}",
                    static_cast<std::uint8_t>(parser_.GetErrorCode()));
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