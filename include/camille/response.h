#ifndef CAMILLE_INCLUDE_CAMILLE_RESPONSE_H_
#define CAMILLE_INCLUDE_CAMILLE_RESPONSE_H_

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

 private:
  std::string_view path_;
  std::string_view method_;
  std::string_view version_;
};

};  // namespace response
};  // namespace camille

#endif