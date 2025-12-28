#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_H_

#include "infra.h"

#include <string>

// TODO: implement: Zero-Copy Parsing

namespace camille {

namespace request {

template <typename T>
class Request {
 public:
  explicit Request(infra::Methods method);
  ~Request() = default;

  bool AddHeader(const std::string& header_name, const std::string& header_value) {}

 private:
  std::string body_;
  std::string status_code_;
  infra::Methods request_method_;
  infra::headers::RequestHeaders headers_;
};

};  // namespace request
};  // namespace camille

#endif