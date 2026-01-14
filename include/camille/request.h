#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_H_

#include "types.h"
#include "infra.h"

#include <string>
#include <utility>

// TODO: implement: Zero-Copy Parsing
// TODO: add main camille headers

namespace camille {

namespace request {

class Request {
 public:
  Request() = default;
  ~Request() = default;

  bool AddHeader(const std::string& header_name, const std::string& header_value);
  //   void SetHeaders(types::camille::CamilleHeaders&& headers) { headers_ = std::move(headers); }
  void SetBody(const std::string& body);

  //   std::string_view GetMethod() { return infra::MethodString(method); }

  std::string method;
  //   infra::Methods method;

 private:
  std::string body_;
  std::string path_;
  size_t request_size_;
  bool has_auth_{false};
  types::camille::CamilleHeaders headers_;
};

};  // namespace request
};  // namespace camille

#endif