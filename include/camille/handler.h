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

template <typename T>
using ParseResponse = std::pair<std::optional<T>, std::optional<error::Errors>>;

class RequestHandler {
 public:
  RequestHandler() = default;

  void PrintRequest() const { request_.PrintRequest(); }

  ParseResponse<request::Request> Parse(std::string_view data, bool is_partial = false) {
    auto req = parser_.Parse<request::Request>(request_, data, is_partial);
    // INFO: apperantly the return value does a problem.
    // request_ = req.value();  // reassign??

    if (req.has_value()) {
      if (parser_.GetErrorCode() == error::Errors::kDefault)
        return {req.value(), error::Errors::kDefault};
      if (is_partial && parser_.GetErrorCode() == error::Errors::kPartialMessage) {
        return {req.value(), error::Errors::kDefault};
      }
    }

    if (req.error() == error::Errors::kPartialMessage) {
      return {std::nullopt, req.error()};
    }

    CAMILLE_ERROR("Request parsing error: {}", static_cast<std::uint8_t>(req.error()));
    return {std::nullopt, req.error()};
  }

 private:
  parser::Parser parser_;
  request::Request request_;
};

class ResponseHandler {
 public:
  ResponseHandler() = default;

  auto Parse(std::string_view data) -> ParseResponse<response::Response> {
    auto res = parser_.Parse<response::Response>(response_, data);
    if (!parser_) {
      CAMILLE_ERROR("Response parsing error: {}",
                    static_cast<std::uint8_t>(parser_.GetErrorCode()));
      // return std::nullopt;
    }
    // return res.value();
  }

 private:
  parser::Parser parser_;
  response::Response response_;
};

};  // namespace handler
};  // namespace camille.

#endif