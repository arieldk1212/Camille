#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "error.h"

#include <cstdint>
#include <expected>
#include <string_view>
#include <variant>

/**
 * @ref request smuggling
 * always add body and be willing to accept one.
 * interact from front and back with the same http version.
 * throw error on exception, dont continue.

* @example
  GET / HTTP/1.1
  Host: 127.0.0.1:8085
  Connection: keep-alive
  sec-ch-ua: "Brave";v="143", "Chromium";v="143", "Not A(Brand";v="24"
  sec-ch-ua-mobile: ?0
  sec-ch-ua-platform: "macOS"
  Upgrade-Insecure-Requests: 1
  User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like
Gecko) Chrome/143.0.0.0 Safari/537.36 Accept:
  text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/
/*;q=0.8
Sec-GPC: 1
Accept-Language: en-US,en;q=0.5
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br, zstd

*/
namespace camille {
namespace parser {

enum class States : std::uint8_t {
  kReady,
  // kWait,

  kMethod,
  // kUri,
  // kVersion,

  // kKey,
  // kValue,

  // kBody,

  kComplete,
  kGarbage
};

constexpr static std::uint64_t kBodyLimit = 64 * 1024;  // 64k total, prob change

using It = std::string_view::iterator;

class Parser {
 public:
  Parser() = default;

  explicit operator bool() const { return std::get<States>(current_state_) == States::kComplete; }

  static char CurrentValue(It& pos) { return *pos; }

  void SetUsed(bool used) { used_ = used; }

  static bool IsEmpty(It& pos, It& end) { return pos == end; }
  static bool IsSpace(char sus) { return true ? sus == ' ' : false; }

  template <typename T>
  bool ParseMethod(auto* pos, T& dtype) {
    while (*pos != ' ') {
      dtype.method.push_back(*pos);
      ++pos;
    }
    return true;
  }

  template <typename T>
  std::expected<T, error::Errors> Parse(std::string_view data) {
    T dtype;
    It data_begin = data.cbegin();
    It data_end = data.cend();

    while (data_begin != data_end) {
      switch (std::get<States>(current_state_)) {
        case States::kReady:
          if (used_) {
            return error::Errors::kStaleParser;
          }
          current_state_ = States::kMethod;
          break;

        case States::kMethod:
          if (!ParseMethod(data_begin, dtype)) {
            current_state_ = States::kGarbage;
          }
          current_state_ = States::kComplete;
          break;

        case States::kComplete:
          SetUsed(true);
          if (!IsEmpty(data_begin, data_end)) {
            return std::unexpected(error::Errors::kStaleParser);
          }
          return dtype;
          break;

        case States::kGarbage:
          return std::unexpected(error::Errors::kGarbageRequest);
          break;

        default:
          return std::unexpected(error::Errors::kGarbageRequest);
      }
    }
    return std::unexpected(error::Errors::kGarbageRequest);
  }

 private:
  bool used_{false};
  size_t total_consumed_{0};
  size_t data_limit_{kBodyLimit};
  std::variant<States> current_state_{States::kReady};
};

};  // namespace parser
};  // namespace camille

#endif