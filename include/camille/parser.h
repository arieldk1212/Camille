#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "benchmark.h"
#include "infra.h"
#include "logging.h"
#include "types.h"
#include "concepts.h"
#include "error.h"

#include <cstdint>
#include <expected>
#include <map>
#include <string_view>

/**
* @example
  GET / HTTP/1.1\r\n
  Host: 127.0.0.1:8085\r\n
  Connection: keep-alive\r\n
  sec-ch-ua: "Brave";v="143", "tokenomium";v="143", "Not A(Brand";v="24"
  sec-ch-ua-mobile: ?0
  sec-ch-ua-platform: "macOS"
  Upgrade-Insecure-Requests: 1
  User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like
Gecko) tokenome/143.0.0.0 Safari/537.36 Accept:
  text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/
/*;q=0.8
Sec-GPC: 1
Accept-Language: en-US,en;q=0.5
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br, zstd

For validation:
1. Must check if a char is a control char (ASCII 0-31, 127).
2. Allow alphanumeric a-z, A-Z, 0-9.
3. Method and headers allow symbols !#$%&'*+-.^_`|~, Forbidden: @:/[] and other
4. URI: Reserved (?#&=), Unreserved (-._~), Percent Encoding (% followed by exactly two hex decimal
digits).
5. OWS(Opt White Space): No whitespace allowed between the key and the colon, leading and trailing
whitespace sohuld be identified so they can be trimmed, vertical tabs and form feeds should usually
be rejected as garbage.
6. A line must end with \r\n, Full request ends with \r\n\r\n.
7. Header values must be in range of 32-126, or extended if support is on for internatiolized
headers.
8. Content-Length and Status Code must be 0-9 digit only!
9. If a request has both Content-Length and Transfer-Encoding, the spec says you must
prioritize Transfer-Encoding or throw a 400 error.
10. If Content-Length is invalid (e.g., 123, 456), you must throw an error. Never "guess" which
length is correct.
11. Always add body and be willing to accept one.
12. Interact from front and back with the same http version.
13. Request Smuggling prevention.

*/
namespace camille {
namespace parser {

enum class States : std::uint8_t {
  kReady,
  kMethod,
  kWaitUri,
  kUriStart,
  kUri,
  kVersion,
  // kKey,
  // kValue,
  // kBody,
  kComplete,
  kGarbage
};

static constexpr std::uint64_t kBodyLimit = 64 * 1024;  // 64k total, prob change

/**
 * @brief Lookup map
 * @todo Implement or ignore
 */
[[maybe_unused]] static const std::map<unsigned char, unsigned char> kBitmap;

static constexpr bool IsSpace(char token) { return token == ' ' || token == '\t'; }
static constexpr bool IsDigit(char token) { return (token >= '0' && token <= '9'); }
static constexpr bool IsLower(char token) {
  return (static_cast<unsigned char>(token) >= 'a' && static_cast<unsigned char>(token) <= 'z');
}
static constexpr bool IsUpper(char token) {
  return (static_cast<unsigned char>(token) >= 'A' && static_cast<unsigned char>(token) <= 'Z');
}
static constexpr bool IsChar(char token) { return token >= 0 && token <= 127; }
static constexpr bool IsControl(char token) {
  return (token >= 0 && token <= 31) || (token == 127);
}
static constexpr bool IsFormFeed(char token) { return token == 0x0C; }
static constexpr bool IsCR(char token) { return token == 0x0D; }
static constexpr bool IsLF(char token) { return token == 0x0A; }
static constexpr bool IsCRLF(std::string_view token) { return token == "\r\n\r\n"; }

class Parser {
 public:
  using It = types::camille::CamilleStringViewIt;

  struct Rocky {
    It begin;
    It end;
    std::string_view data;
  };

 public:
  Parser() = default;

  explicit operator bool() const { return current_state_ == States::kComplete; }

  void SetUsed(bool used) { used_ = used; }
  [[nodiscard]] bool IsEmpty() const { return rocky_.begin == rocky_.end; }

  template <concepts::IsReqResType T>
  static bool ParseMethod(auto& pos, T& dtype) {
    auto begin = pos;
    while (!IsSpace(*pos) || !IsLower(*pos)) {
      ++pos;
    }
    std::string_view method(begin, pos - begin);
    if (infra::MethodEnum(method) == infra::Methods::kUnknown) {
      return false;
    }
    dtype.SetMethod(method);
    return true;
  }

  template <concepts::IsReqResType T>
  static bool ParseUri(auto& pos, T& dtype) {
    CAMILLE_DEBUG("Parse Uri Executed");
    auto begin = pos;
    if (!(*begin) == '/') {
      return false;
    }

    while (!IsSpace(*pos)) {
      if () }

    std::string_view path(begin, pos - begin);
    dtype.SetPath(path);
    return true;
  }

  template <concepts::IsReqResType T>
  [[nodiscard]] std::expected<T, error::Errors> Parse(std::string_view data) {
    T dtype;
    rocky_.begin = data.cbegin();
    rocky_.end = data.cend();
    rocky_.data = data;

    while (rocky_.begin != rocky_.end) {
      switch (current_state_) {
        case States::kReady:
          if (used_) {
            return std::unexpected(error::Errors::kStaleParser);
          }
          current_state_ = States::kMethod;
          break;

        case States::kMethod:
          if (!ParseMethod(rocky_.begin, dtype)) {
            error_ = error::Errors::kBadMethod;
            current_state_ = States::kGarbage;
          } else {
            current_state_ = States::kWaitUri;
          }
          break;

        case States::kWaitUri:
          if (IsSpace(*rocky_.begin)) {
            ++rocky_.begin;
            current_state_ = States::kUriStart;
          } else {
            error_ = error::Errors::kBadRequest;
            current_state_ = States::kGarbage;
          }
          break;

        case States::kUriStart:
          if (*rocky_.begin == '/') {
            current_state_ = States::kUri;
          } else {
            error_ = error::Errors::kBadUri;
            current_state_ = States::kGarbage;
          }
          break;

        case States::kUri:
          if (!ParseUri(rocky_.begin, dtype)) {
            error_ = error::Errors::kBadUri;
            current_state_ = States::kGarbage;
          } else {
            current_state_ = States::kVersion;
          }
          break;

        case States::kVersion:

          break;

        case States::kComplete:
          SetUsed(true);
          // if (!IsEmpty()) {
          //   return std::unexpected(error::Errors::kStaleParser);
          // }
          // dtype.SetSize(total_consumed_);
          return dtype;
          break;

        case States::kGarbage:
          return std::unexpected(error_);
          break;

        default:
          return std::unexpected(error::Errors::kGeneralError);
      }
    }
    return std::unexpected(error_);
  }

 private:
  Rocky rocky_;
  bool used_{false};
  size_t total_consumed_{0};
  size_t data_limit_{kBodyLimit};
  States current_state_{States::kReady};
  error::Errors error_{error::Errors::kGeneralError};
};

};  // namespace parser
};  // namespace camille

#endif