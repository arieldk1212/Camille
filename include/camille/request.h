#ifndef CAMILLE_INCLUDE_CAMILLE_REQUEST_H_
#define CAMILLE_INCLUDE_CAMILLE_REQUEST_H_

#include <optional>

#include "types.h"
#include "logging.h"

namespace camille {
namespace request {

class Request {
 public:
  Request() = default;

  [[nodiscard]] std::string_view Host() const { return host_; }
  void SetHost(std::string_view host) { host_ = std::string(host); }

  [[nodiscard]] std::string_view Port() const { return port_; }
  void SetPort(std::string_view port) { port_ = std::string(port); }

  [[nodiscard]] std::string_view Path() const { return path_; }
  void SetPath(std::string_view path) { path_ = std::string(path); }

  [[nodiscard]] std::string_view Body() const { return body_; }
  void SetBody(std::string_view body) { body_ = std::string(body); }

  [[nodiscard]] std::string_view Method() const { return method_; }
  void SetMethod(std::string_view method) { method_ = std::string(method); }

  [[nodiscard]] std::string_view Version() const { return version_; }
  void SetVersion(std::string_view version) { version_ = std::string(version); }

  [[nodiscard]] size_t ContentLength() const { return content_length_; }
  void SetContentLength(size_t content_length) { content_length_ = content_length; }

  [[nodiscard]] const types::camille::CamilleHeaders& Headers() const { return headers_; }
  void AddHeader(std::string_view key, std::string_view value) {
    headers_.emplace_back(std::string(key), std::string(value));
  }
  /**
   * @brief Get the Header object (checks for duplicates and emptiness)
   * @param header_key
   * @return std::optional<std::string_view>
   */
  [[nodiscard]] std::optional<std::string_view> GetHeader(std::string_view header_key) const {
    int dup{0};
    std::string_view header_value{};

    for (const auto& [key, value] : headers_) {
      if (header_key == key) {
        header_value = std::string_view(value);
        ++dup;
      }
    }

    if (dup == 1) {
      return header_value;
    }
    return std::nullopt;
  }

  [[nodiscard]] bool Auth() const { return has_auth_; }
  void SetAuth(bool auth) { has_auth_ = auth; }

  [[nodiscard]] size_t Size() const { return request_size_; }
  void SetSize(size_t size) { request_size_ = size; }
  void AddSize(size_t size) { request_size_ += size; }

  void PrintRequest() const {
    CAMILLE_DEBUG("Method: {}", method_);
    CAMILLE_DEBUG("Uri: {}", path_);
    CAMILLE_DEBUG("Version: {}", version_);
    CAMILLE_DEBUG("Host: {}", host_);
    CAMILLE_DEBUG("Port: {}", port_);
    for (const auto& [key, value] : headers_) {
      CAMILLE_DEBUG("Header Key: {}", key);
      CAMILLE_DEBUG("Header Value: {}", value);
    }
    CAMILLE_DEBUG("Size: {}", request_size_);
    CAMILLE_DEBUG("Content-Length: {}", content_length_);
    CAMILLE_DEBUG("Body: {}", body_);
  }

 private:
  std::string host_;
  std::string port_;
  std::string path_;
  std::string body_;
  std::string method_;
  std::string version_;
  size_t content_length_{0};
  types::camille::CamilleHeaders headers_;

  bool has_auth_{false};
  size_t request_size_{0};
};

};  // namespace request
};  // namespace camille

#endif