#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_H_

#include "types.h"

/**
 * @example
 * POST /api/v1/update-profile?session_id=992834 HTTP/1.1
   Host: 127.0.0.1:8085
   Connection: keep-alive
   Content-Length: 54
   Content-Type: application/json; charset=UTF-8
   User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like
 Gecko) Chrome/143.0.0.0 Safari/537.36 Accept: application/json, text/plain, *//*
   Sec-CH-UA: "Brave";v="143", "Chromium";v="143", "Not A(Brand";v="24"
   Sec-CH-UA-Platform: "macOS"
   Accept-Encoding: gzip, deflate, br, zstd
   Accept-Language: en-US,en;q=0.9
   Referer: http://127.0.0.1:8085/settings/profile
   {
     "username": "camille_dev",
     "status": "active"
   }
 */

namespace camille {
namespace request {

/**
 * @todo method to extract request into a const data struct.
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

  [[nodiscard]] std::string_view Version() const { return version_; }
  void SetVersion(std::string_view version) { version_ = version; }

  [[nodiscard]] const types::camille::CamilleViewHeaders& Headers() const { return headers_; }
  void AddHeader(std::string_view key, std::string_view value) {
    headers_.emplace_back(key, value);
  }

  [[nodiscard]] bool Auth() const { return has_auth_; }
  void SetAuth(bool auth) { has_auth_ = auth; }

  [[nodiscard]] size_t Size() const { return request_size_; }
  void SetSize(size_t size) { request_size_ = size; }
  void AddSize(size_t size) { request_size_ += size; }

 private:
  std::string_view host_;
  std::string_view port_;
  std::string_view path_;
  std::string_view body_;
  std::string_view method_;
  std::string_view version_;
  types::camille::CamilleViewHeaders headers_;

  bool has_auth_{false};
  size_t request_size_{0};
};

};  // namespace request
};  // namespace camille

#endif