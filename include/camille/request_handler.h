#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_HANDLER_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_HANDLER_H_

#include "request.h"

namespace camille {
namespace request_handler {

class RequestHandler {
 public:
  RequestHandler();

  void Process();

 private:
};

};  // namespace request_handler
};  // namespace camille

#endif