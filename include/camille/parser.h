#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "benchmark.h"
#include "infra.h"
#include "logging.h"
#include "types.h"
#include "concepts.h"
#include "error.h"

#include <atomic>
#include <cstdint>
#include <array>
#include <expected>
#include <string_view>

/**
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

/**
 * @todo at the end when finished, add rules and
 * make sure it wont be overwritten.
 * also clean the functions, make the code more readable.
 */

namespace camille {
namespace parser {

enum class States : std::uint8_t {
  kReady,
  kMethod,
  kWaitUri,
  kUriStart,
  kUri,
  kWaitVersion,
  kVersion,
  kHeadersWait,
  kHeaders,
  kKey,
  kValue,
  // kBody,
  kComplete,
  kGarbage
};

static constexpr std::uint64_t kTableLimit = 256;
static constexpr std::uint64_t kBodyLimit = 64 * 1024;  // 64k total, prob change

/**
 * @brief Lookup map
 * @todo Decide on the implementation
 */

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
static constexpr bool IsSlash(char token) { return token == '/'; }
static constexpr bool IsOWS(char token) {
  return static_cast<bool>(std::isspace(static_cast<unsigned char>(token)));
}
static constexpr bool IsFormFeed(char token) { return token == 0x0C; }
static constexpr bool IsCR(char token) { return token == 0x0D; }
static constexpr bool IsLF(char token) { return token == 0x0A; }
static constexpr bool IsCRLF(std::string_view token) { return token == "\r\n"; }
static constexpr bool IsEndOfHeaders(std::string_view token) { return token == "\r\n\r\n"; }

class Parser {
 public:
  /**
   * @brief Responsible for validating a certain token in the specific use-case.
   * @tparam requirements (concept that checks for 'placements' array inside, holding the symbols as
   * decimals)
   */
  template <concepts::SymbolRequirement... requirements>
  struct ParserTraits {
    static constexpr std::array<char, 256> kLookup = []() consteval {
      std::array<char, kTableLimit> table{};
      (
          [&]() {
            for (auto token : requirements::placements) {
              table[static_cast<unsigned char>(token)] = token;
            }
          }(),
          ...);
      return table;
    }();

    template <std::same_as<char> T>
    [[nodiscard]] static constexpr bool IsValid(T token) {
      return kLookup[static_cast<unsigned char>(token)];
    }
  };
  struct CommonSymbolRequirement {
    static constexpr auto placements = []() {
      std::array<char, 33> arr{};
      for (char ch{0}; ch <= 31; ++ch) {
        arr.at(ch) = ch;
      }
      arr[32] = 127;
      return arr;
    }();
  };
  struct AlphaNumericRequirement {
    static constexpr std::array<char, 62> placements{
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
  };
  struct HeaderSymbolRequirement {
    static constexpr std::array<char, 16> placements{'!', '#', '$', '%', '&', '\'', '*', '+',
                                                     ',', '-', '.', '^', '_', '`',  '|', '~'};
  };
  struct HeaderValueRequirement {
    static constexpr auto placements = []() {
      std::array<char, 95> arr{};
      size_t iter{0};
      for (char ch = 32; ch <= 126; ++ch) {
        // arr[ch - 32] = ch;
        arr.at(iter) = ch;
        ++iter;
      }
      return arr;
    }();
  };
  struct ReservedSymbolRequirement {
    static constexpr std::array<char, 4> placements{'#', '&', '=', '?'};
  };
  struct UnreservedSymbolRequirement {
    static constexpr std::array<char, 4> placements{'-', '.', '_', '~'};
  };

  using It = types::camille::CamilleStringViewIt;
  using UriTraits = ParserTraits<UnreservedSymbolRequirement, ReservedSymbolRequirement>;
  using HeaderTraits = ParserTraits<CommonSymbolRequirement,
                                    HeaderSymbolRequirement,
                                    HeaderValueRequirement,
                                    AlphaNumericRequirement>;

  struct Rocky {
    It begin;
    It end;
    std::string_view data;
  };

 public:
  Parser() = default;

  explicit operator bool() const { return current_state_ == States::kComplete; }

  void SetUsed() { used_ = true; }
  [[nodiscard]] bool IsEmpty() const { return rocky_.begin == rocky_.end; }
  [[nodiscard]] std::uint8_t GetErrorCode() const { return static_cast<std::uint8_t>(error_); }
  [[nodiscard]] std::string_view GetErrorString() const { return error::ErrorToString(error_); }

  template <concepts::IsReqResType T>
  static bool ParseMethod(auto& pos, T& dtype) {
    auto begin = pos;
    while (!IsSpace(*pos) && !IsLower(*pos)) {
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
    auto begin = pos;
    if (!IsSlash(*pos)) {
      return false;
    }
    ++pos;

    while (!IsSpace(*pos)) {
      if (!IsChar(*pos) && !UriTraits::IsValid(*pos) && !IsSlash(*pos)) {
        return false;
      }
      ++pos;
    }

    std::string_view path(begin, pos - begin);
    dtype.SetPath(path);
    return true;
  }

  template <concepts::IsReqResType T>
  static bool ParseVersion(auto& pos, T& dtype) {
    auto begin = pos;
    while (!IsSlash(*pos)) {
      if (!IsChar(*pos) || !IsUpper(*pos)) {
        return false;
      }
      ++pos;
    }
    std::string_view http(begin, pos - begin);
    if (http != "HTTP") {
      return false;
    }
    ++pos;
    while (!IsCR(*pos)) {
      if (!IsDigit(*pos) && *pos != '.') {
        return false;
      }
      ++pos;
    }
    ++pos;  // now at '\n'
    std::string_view version(begin, pos - begin);
    dtype.SetVersion(version);
    return true;
  }

  template <concepts::IsReqResType T>
  static void ParseHost(std::string_view value, T& dtype) {
    const char* pos = value.begin();
    while (pos != value.end() && *pos != ':') {
      ++pos;
    }
    std::string_view host(value.begin(), pos - value.begin());
    ++pos;
    std::string_view port(pos, value.end() - pos);
    dtype.SetHost(host);
    dtype.SetPort(port);
  }

  template <concepts::IsReqResType T>
  static bool ParseHeaders(auto& pos, T& dtype) {
    It begin{};
    bool can_consume_more{true};
    while (can_consume_more) {
      begin = pos;
      while (*pos != ':') {
        if (HeaderTraits::IsValid(*pos)) {
          ++pos;
        } else {
          return false;
        }
      }
      std::string_view current_key(begin, pos - begin);
      ++pos;
      if (IsSpace(*pos)) {
        ++pos;
      }
      begin = pos;
      while (!IsCR(*pos)) {
        if (HeaderTraits::IsValid(*pos)) {
          ++pos;
        } else {
          return false;
        }
      }
      std::string_view current_value(begin, pos - begin);
      dtype.AddHeader(current_key, current_value);
      if (current_key.size() == 4 && (current_key[0] == 'H' || current_key[0] == 'h') &&
          (current_key[1] == 'O' || current_key[1] == 'o') &&
          (current_key[2] == 'S' || current_key[2] == 's') &&
          (current_key[3] == 'T' || current_key[3] == 't')) {
        ParseHost(current_value, dtype);
      }

      begin = pos;
      std::atomic<std::uint8_t> count{};

      while (IsCR(*pos) || IsLF(*pos)) {
        count.fetch_add(1, std::memory_order_relaxed);
        ++pos;
      }
      if (count == 4) {
        can_consume_more = false;
      }
      /**
       if (pos + 2 <= (end) && IsCR(*pos) && IsLF(*(pos + 1))) {
        +pos += 2;
        +if (pos + 2 <= (end) &&IsCR(*pos) && IsLF(*(pos + 1))) {
          +pos += 2;
          +can_consume_more = false;
          +
        }
        +
      }
      else {
        can_consume_more = false;
      }
      */
    }
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
          if (IsSlash(*rocky_.begin)) {
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
            current_state_ = States::kWaitVersion;
          }
          break;

        case States::kWaitVersion:
          if (IsSpace(*rocky_.begin)) {
            ++rocky_.begin;
            current_state_ = States::kVersion;
          } else {
            error_ = error::Errors::kBadRequest;
            current_state_ = States::kGarbage;
          }
          break;

        case States::kVersion:
          if (!ParseVersion(rocky_.begin, dtype)) {
            error_ = error::Errors::kBadVersion;
            current_state_ = States::kGarbage;
          } else {
            current_state_ = States::kHeadersWait;
          }
          break;

        case States::kHeadersWait:
          if (IsLF(*rocky_.begin)) {
            ++rocky_.begin;
            current_state_ = States::kHeaders;
          } else {
            error_ = error::Errors::kPartialMessage;
            current_state_ = States::kGarbage;
          }
          break;

        case States::kHeaders:
          if (!ParseHeaders(rocky_.begin, dtype)) {
            error_ = error::Errors::kBadKey;
            current_state_ = States::kGarbage;
          } else {
            current_state_ = States::kComplete;
            return dtype;  // why? begin == end, next it will fail.
          }
          break;

        case States::kComplete:
          SetUsed();
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