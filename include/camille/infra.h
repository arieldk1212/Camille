#ifndef CAMILLE_INCLUDE_CAMILLE_INFRA_H_
#define CAMILLE_INCLUDE_CAMILLE_INFRA_H_

/**
 * @brief put here the headers, types, uri, status codes.
 */

#include <unordered_map>
#include <string>

namespace infra {

enum class Methods : std::uint8_t { GET, HEAD, POST, PATCH, PUT, DELETE, OPTIONS, CONNECT, TRACE };
enum class LogLevels : std::uint8_t { DEBUG, INFO, WARNING, ERROR, CRITICAL };

namespace types {

using Headers = std::unordered_map<std::string, std::string>;

};  // namespace types

namespace headers {

class Headers {
 public:
  Headers();

  bool AddHeader(const std::string& name, const std::string& value);

 private:
  types::Headers headers_;
};

class TypeHeaders {
 public:
  virtual ~TypeHeaders() = default;

  virtual bool MatchToType() = 0;
};

class RequestHeaders : public TypeHeaders {
 public:
  RequestHeaders();

  bool MatchToType() override;

 private:
  std::string request_line_;
  Headers headers_;
};

class ResponseHeaders : public TypeHeaders {
 public:
  ResponseHeaders();

  bool MatchToType() override;

 private:
  std::string response_line_;
  Headers headers_;
};

};  // namespace headers

namespace uri {

class Uri {};

class Url {};

};  // namespace uri

};  // namespace infra

#endif