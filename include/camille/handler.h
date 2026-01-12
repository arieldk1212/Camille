#ifndef CAMILLE_INCLUDE_CAMILLE_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_HANDLER_H_

#include "request.h"
#include "response.h"

namespace camille {
namespace handler {

class Handler {
 public:
  Handler();

  [[nodiscard("Response is waiting")]] static response::Response Process(const std::string& data) {
    response::Response response;
    return response;
  }

 private:
  void Parser();
};

};  // namespace handler
};  // namespace camille.

#endif