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

  const request::Request& Parse(std::string_view data) {
    auto req = parser_.Parse<request::Request>(data);
    if (!parser_) {
      CAMILLE_ERROR("request parsing error");
    }
    request_ = req.value();
    return request_;
  }

 private:
  parser::Parser parser_;
  request::Request request_;
};

class ResponseHandler : public Handler {
 public:
  ResponseHandler() = default;

  const response::Response& Parse(std::string_view data) {
    auto res = parser_.Parse<response::Response>(data);
    if (!parser_) {
      CAMILLE_ERROR("response parsing error");
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