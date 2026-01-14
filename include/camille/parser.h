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

 * @brief Parser implementation via a state machine
 * flow:
 * data starts at method, base state is Ready, once we consume the method,
 * state gets set to Uri, we append the bytes into the total consumed,
 * state is then set to wait until we are done with method, after that we set
 * the state to ready, consume uri, till we reach a char and not " ".

 * @arg std::string_view data
 *      pointer to the data itself, non-copy, caret goes along with it.
 */

namespace camille {
namespace parser {

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
  explicit Parser(std::string_view data, request::Request& request)
      : data_(data),
        request_(request) {}

  explicit operator bool() const { return std::get<States>(current_state_) == States::kComplete; }

  void SetState(States state) { current_state_ = state; }

  bool ParseMethod(auto* pos) {
    while (*pos != ' ') {
      request_.method.push_back(*pos);
      ++pos;
    }
    return true;
  }

  std::expected<request::Request, States> StateMachine() {
    std::string_view::const_iterator it_begin = data_.cbegin();
    std::string_view::const_iterator it_end = data_.cend();

    while (it_begin != it_end) {
      switch (std::get<States>(current_state_)) {
        case States::kReady:
          current_state_ = States::kMethod;
          break;

        case States::kMethod:
          if (!ParseMethod(it_begin)) {
            current_state_ = States::kGarbage;
          }
          current_state_ = States::kComplete;
          break;

        case States::kComplete:
          return request_;
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
  std::string_view data_;
  request::Request request_;
  size_t total_consumed_{0};
  size_t data_limit_{kBodyLimit};
  std::variant<States> current_state_{States::kReady};
};

};  // namespace parser
};  // namespace camille

#endif