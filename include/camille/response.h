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
  void SetHost(std::string_view host) { host_ = std::string(host); }

  [[nodiscard]] std::string_view Port() const { return port_; }
  void SetPort(std::string_view port) { port_ = std::string(port); }

  [[nodiscard]] std::string_view Method() const { return method_; }
  void SetMethod(std::string_view method) { method_ = std::string(method); }

  [[nodiscard]] std::string_view Path() const { return path_; }
  void SetPath(std::string_view path) { path_ = std::string(path); }

  [[nodiscard]] std::string_view Version() const { return version_; }
  void SetVersion(std::string_view version) { version_ = std::string(version); }

  [[nodiscard]] const types::camille::CamilleHeaders& Headers() const { return headers_; }
  void AddHeader(std::string_view key, std::string_view value) {
    headers_.emplace_back(std::string(key), std::string(value));
  }
  std::optional<std::string_view> GetHeader(std::string_view header_key) {
    /**
     * @todo O(n).. no need to cache.. or yes? benchmark.
     */
    for (const auto& [key, value] : headers_) {
      if (header_key == key) {
        return std::string_view(value);
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] size_t Size() const { return response_size; }
  void SetSize(size_t size) { response_size = size; }
  void AddSize(size_t size) { response_size += size; }

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
  std::string host_;
  std::string port_;
  std::string path_;
  std::string method_;
  std::string version_;
  types::camille::CamilleHeaders headers_;

  size_t response_size{0};
};

};  // namespace response
};  // namespace camille

#endif