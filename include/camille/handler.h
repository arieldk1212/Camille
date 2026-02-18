#ifndef CAMILLE_INCLUDE_CAMILLE_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_HANDLER_H_

#include <expected>
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

  std::expected<request::Request, error::Errors> Parse(std::string_view data,
                                                       bool is_partial = false) {
    auto req = parser_.Parse<request::Request>(request_, data, is_partial);

    // TODO: we need to make this better, the return statements from the parser..
    if (req.has_value()) {
      if ((parser_.GetErrorCode() == error::Errors::kDefault) ||
          (is_partial && parser_.GetErrorCode() == error::Errors::kPartialMessage)) {
        return request_;
      }
    }
    if (req.value() == error::Errors::kPartialMessage) {
      return std::unexpected(req.value());
    }

    CAMILLE_ERROR("Request parsing error: {}", static_cast<std::uint8_t>(req.value()));
    return std::unexpected(req.value());
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