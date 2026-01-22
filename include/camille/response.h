#ifndef CAMILLE_INCLUDE_CAMILLE_RESPONSE_H_
#define CAMILLE_INCLUDE_CAMILLE_RESPONSE_H_

#include "types.h"

#include <string_view>

namespace camille {
namespace response {

class Response {
 public:
  Response() = default;

  [[nodiscard]] std::string_view Method() const { return method_; }
  void SetMethod(std::string_view method) { method_ = method; }

  [[nodiscard]] std::string_view Path() const { return path_; }
  void SetPath(std::string_view path) { path_ = path; }

  [[nodiscard]] std::string_view Version() const { return version_; }
  void SetVersion(std::string_view version) { version_ = version; }

  [[nodiscard]] const types::camille::CamilleViewHeaders& Headers() const { return headers_; }
  void AddHeader(std::string_view key, std::string_view value) {
    headers_.emplace_back(key, value);
  }

 private:
  std::string_view path_;
  std::string_view method_;
  std::string_view version_;
  types::camille::CamilleViewHeaders headers_;
};

};  // namespace response
};  // namespace camille

#endif