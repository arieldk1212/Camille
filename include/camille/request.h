#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_H_

#include "types.h"

#include <string>

namespace camille {
namespace request {

/**
 * @todo add some sort of validation of the methods enum
 */
class Request {
 public:
  Request() = default;

  [[nodiscard]] std::string_view Host() const { return host_; }
  void SetHost(std::string_view host) { host_ = host; }

  [[nodiscard]] std::string_view Port() const { return port_; }
  void SetPort(std::string_view port) { port_ = port; }

  [[nodiscard]] std::string_view Path() const { return path_; }
  void SetPath(std::string_view path) { path_ = path; }

  [[nodiscard]] std::string_view Body() const { return body_; }
  void SetBody(std::string_view body) { body_ = body; }

  [[nodiscard]] std::string_view Method() const { return method_; }
  void SetMethod(std::string_view method) { method_ = method; }
  void SetMethod(std::string method) { method_ = std::move(method); }

  [[nodiscard]] const types::camille::CamilleHeaders& Headers() const { return headers_; }
  void AddHeader(std::string_view key, std::string_view value) {
    headers_.emplace_back(std::string(key), std::string(value));
  }

  [[nodiscard]] bool Auth() const { return has_auth_; }
  void SetAuth(bool auth) { has_auth_ = auth; }

  [[nodiscard]] size_t Size() const { return request_size_; }
  void SetSize(size_t size) { request_size_ = size; }
  void AddSize(size_t size) { request_size_ += size; }

 private:
  std::string host_;
  std::string port_;
  std::string path_;
  std::string body_;
  std::string method_;
  types::camille::CamilleHeaders headers_;

  bool has_auth_{false};
  size_t request_size_{0};
};

};  // namespace request
};  // namespace camille

#endif