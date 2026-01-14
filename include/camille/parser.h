#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "request.h"
#include "logging.h"
#include "types.h"

#include <cstdint>
#include <expected>
#include <print>
#include <string_view>
#include <variant>

/**
 * @ref request smuggling
 * always add body and be willing to accept one.
 * interact from front and back with the same http version.
 * throw error on exception, dont continue.
 */
namespace camille {
namespace parser {

using ItDataConst = std::string_view::const_iterator;

constexpr static std::uint64_t kBodyLimit = 64 * 1024;  // 64k total, prop change

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

class Parser {
 public:
  Parser() = default;

  explicit operator bool() const { return std::get<States>(current_state_) == States::kComplete; }

  template <typename Type = request::Request>
  bool ParseMethod(auto* pos, Type& dtype) {
    while (*pos != ' ') {
      dtype.method.push_back(*pos);
      ++pos;
    }
    return true;
  }

  template <typename Type = request::Request>
  std::expected<Type, States> Parse(std::string_view data, Type& dtype) {
    ItDataConst data_begin = data.cbegin();
    ItDataConst data_end = data.cend();

    while (data_begin != data_end) {
      switch (std::get<States>(current_state_)) {
        case States::kReady:
          current_state_ = States::kMethod;
          break;

        case States::kMethod:
          if (!ParseMethod(data_begin, dtype)) {
            current_state_ = States::kGarbage;
          }
          current_state_ = States::kComplete;
          break;

        case States::kComplete:
          return dtype;
          break;

        case States::kGarbage:
          return std::unexpected<States>(States::kGarbage);
          break;

        default:
          return std::unexpected<States>(States::kGarbage);
      }
    }
    return std::unexpected<States>(States::kGarbage);
  }

 private:
  size_t total_consumed_{0};
  size_t data_limit_{kBodyLimit};
  std::variant<States> current_state_{States::kReady};
};

};  // namespace parser
};  // namespace camille

#endif