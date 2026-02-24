#ifndef CAMILLE_INCLUDE_CAMILLE_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_HANDLER_H_

#include <expected>
#include <optional>

#include "camille/infra.h"
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

  std::expected<request::Request, std::pair<infra::States, error::Errors>> Parse(
      std::string_view data, bool is_partial = false) {
    auto parse_result = parser_.Parse<request::Request>(request_, data, is_partial);

    if (!parser_) {
      if ((parse_result.first == infra::States::kBodyChunked ||
           parse_result.first == infra::States::kBodyIdentify) &&
          parse_result.second == error::Errors::kPartialMessage) {
        if (is_partial) {
          request_.SetPartial(true);
        }
        return request_;
      }
      CAMILLE_ERROR("Request parsing error: {}", static_cast<std::uint8_t>(parse_result.second));
      return std::unexpected(parse_result);
    }
    return request_;
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
      // CAMILLE_ERROR("Response parsing error: {}",
      //               static_cast<std::uint8_t>(parser_.GetErrorCode()));
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