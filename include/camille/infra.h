#ifndef CAMILLE_INCLUDE_CAMILLE_INFRA_H_
#define CAMILLE_INCLUDE_CAMILLE_INFRA_H_

#include <unordered_map>
#include <string>

namespace camille {

namespace infra {

enum class Methods : std::uint8_t { GET, HEAD, POST, PATCH, PUT, DELETE, OPTIONS, CONNECT, TRACE };
enum class LogLevels : std::uint8_t { DEBUG, INFO, WARNING, ERROR, CRITICAL };
enum class NetworkError : std::uint8_t { Timeout, ConnectionReset, QuotaExceeded };
// TODO: need to fill the rest..
enum class StatusCodes : std::uint8_t { HTTP_100, HTTP_200, HTTP_300, HTTP_400, HTTP_500 };

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
  Headers headers_;
  std::string request_line_;
};

class ResponseHeaders : public TypeHeaders {
 public:
  ResponseHeaders();

  bool MatchToType() override;

 private:
  Headers headers_;
  std::string response_line_;
};

};  // namespace headers

namespace uri {

class Uri {};

class Url {};

};  // namespace uri

};  // namespace infra

};  // namespace camille

#endif