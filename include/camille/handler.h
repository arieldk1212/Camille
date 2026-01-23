#ifndef CAMILLE_INCLUDE_CAMILLE_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_HANDLER_H_

#include "camille/logging.h"
#include "request.h"
#include "response.h"
#include "parser.h"

namespace camille {
namespace handler {

class Handler {
 public:
  virtual ~Handler() = default;
};

class RequestHandler : public Handler {
 public:
  RequestHandler() = default;

  request::Request Parse(std::string_view data) {
    auto req = parser_.Parse<request::Request>(data);
    if (!parser_) {
      CAMILLE_ERROR("request parsing error - {}: ecode: {}", parser_.GetErrorString(),
                    parser_.GetErrorCode());
    }
    if (req.has_value()) {
      return req.value();
    }
    CAMILLE_ERROR("Parser error, no value found");
  }

 private:
  parser::Parser parser_;
};

class ResponseHandler : public Handler {
 public:
  ResponseHandler() = default;

  const response::Response& Parse(std::string_view data) {
    auto res = parser_.Parse<response::Response>(data);
    if (!parser_) {
      CAMILLE_ERROR("response parsing error - {}: ecode: {}", parser_.GetErrorString(),
                    parser_.GetErrorCode());
    }
    response_ = res.value();
    return response_;
  }

 private:
  parser::Parser parser_;
  response::Response response_;
};

};  // namespace handler
};  // namespace camille.

#endif