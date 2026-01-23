#ifndef CAMILLE_INCLUDE_CAMILLE_RESPONSE_H_
#define CAMILLE_INCLUDE_CAMILLE_RESPONSE_H_

#include "types.h"
#include "logging.h"

namespace camille {
namespace response {

class Response {
 public:
  Response() = default;

  [[nodiscard]] std::string_view Host() const { return host_; }
  void SetHost(std::string_view host) { host_ = host; }

  [[nodiscard]] std::string_view Port() const { return port_; }
  void SetPort(std::string_view port) { port_ = port; }

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

  void PrintResponse() const {
    CAMILLE_DEBUG("Method: {}", method_);
    CAMILLE_DEBUG("Uri: {}", path_);
    CAMILLE_DEBUG("Version: {}", version_);
    CAMILLE_DEBUG("Host: {}", host_);
    CAMILLE_DEBUG("Port: {}", port_);
    for (const auto& [key, value] : headers_) {
      CAMILLE_DEBUG("Header Key: {}", key);
      CAMILLE_DEBUG("Header Value: {}", value);
    }
  }

 private:
  std::string_view host_;
  std::string_view port_;
  std::string_view path_;
  std::string_view method_;
  std::string_view version_;
  types::camille::CamilleViewHeaders headers_;
};

};  // namespace response
};  // namespace camille

#endif