#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_H_

#include "types.h"
#include "infra.h"

#include <string>

// TODO: implement: Zero-Copy Parsing
// TODO: add main camille headers

namespace camille {

namespace request {

class Request {
 public:
  Request(infra::Methods method, const std::string& path);
  Request();
  ~Request() = default;

  bool AddHeader(const std::string& header_name, const std::string& header_value);

 private:
  bool has_auth_{false};
  std::string body_;
  std::string path_;
  infra::Methods method_;
  size_t request_size_;
  types::camille::CamilleHeaders headers_;
};

};  // namespace request
};  // namespace camille

#endif