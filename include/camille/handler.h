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
    parser_.Parse<request::Request>(data);
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
    parser_.Parse<response::Response>(data);
    return response_;
  }

 private:
  parser::Parser parser_;
  response::Response response_;
};

};  // namespace handler
};  // namespace camille.

#endif