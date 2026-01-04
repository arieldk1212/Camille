#ifndef CAMILLE_INCLUDE_CAMILLE_INFRA_H_
#define CAMILLE_INCLUDE_CAMILLE_INFRA_H_

#include <unordered_map>
#include <vector>
#include <string>

namespace camille {

namespace infra {

enum class Methods : std::uint8_t { GET, HEAD, POST, PATCH, PUT, DELETE, OPTIONS, CONNECT, TRACE };
enum class NetworkError : std::uint8_t { Timeout, RemoteClosed, ConnectionReset, QuotaExceeded };

// TODO: need to fill the rest, change the name to the error itself
enum class StatusCodes : std::uint16_t {
  HTTP_100 = 100,
  HTTP_200 = 200,
  HTTP_300 = 300,
  HTTP_400 = 400,
  HTTP_500 = 500
};

namespace types {

using Headers = std::unordered_map<std::string, std::string>;
using ParserData = std::vector<std::tuple<std::string, std::string, std::string>>;

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