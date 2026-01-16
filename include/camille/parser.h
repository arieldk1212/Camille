#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "benchmark.h"
#include "types.h"
#include "concepts.h"
#include "error.h"

#include <cstdint>
#include <expected>
#include <iterator>
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

Rule 1: If a request has both Content-Length and Transfer-Encoding, the spec says you must
prioritize Transfer-Encoding or throw a 400 error.

Rule 2: If Content-Length is invalid (e.g., 123, 456), you must throw an error. Never "guess" which
length is correct.

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

  static constexpr bool IsSpace(char chr) { return chr == ' ' || chr == '\t'; }

  void SetUsed(bool used) { used_ = used; }
  [[nodiscard]] bool IsEmpty() const { return rocky_.begin == rocky_.end; }

  template <concepts::IsReqResType T>
  bool ParseMethod(auto& pos, T& dtype) {
    auto begin = pos;
    while (!IsSpace(*pos)) {
      ++pos;
    }
    dtype.SetMethod(std::string_view(begin, pos - begin));
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
            current_state_ = States::kGarbage;
          } else {
            current_state_ = States::kComplete;  // state == uri
          }
          break;

        case States::kComplete:
          SetUsed(true);
          // if (!IsEmpty()) {
          //   return std::unexpected(error::Errors::kStaleParser);
          // }
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
  Rocky rocky_;
  bool used_{false};
  size_t total_consumed_{0};
  size_t data_limit_{kBodyLimit};
  States current_state_{States::kReady};
};

};  // namespace parser
};  // namespace camille

#endif