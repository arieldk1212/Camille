#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "infra.h"
#include "types.h"
#include "concepts.h"
#include "error.h"

#include <cstddef>
#include <cstdint>
#include <array>
#include <expected>
#include <string_view>

/**
For validation:
8. Content-Length and Status Code must be 0-9 digit only!
10. If Content-Length is invalid (e.g., 123, 456), you must throw an error. Never "guess" which
length is correct.
13. Request Smuggling prevention.
*/

/**
 * @todo
 * 1. make the parser generic for file uploads, json (via templates), use a constexpr model for the
 * size limits and other restrictions.
 * 2. finish the parser and the body consumption, add validation if it exceeds the size limits.
 * 3. keep a const http version that cross-validates it with the frontend.
 * 4. finish Transfer-Encoding & Content-Length.
 */

namespace camille {
namespace parser {

enum class States : std::uint8_t {
  kReady,
  // kFileUpload,
  // kDataUpload,
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
  kBodyIdentify,
  kBodyChunked,
  kComplete,
  kGarbage
};

static constexpr std::uint64_t kTableLimit = 256;
static constexpr std::uint64_t kBodyLimit = 64 * 1024;

static constexpr bool IsSpace(char token) { return token == ' ' || token == '\t'; }
static constexpr bool IsDigit(char token) { return (token >= '0' && token <= '9'); }
static constexpr bool IsHexDigit(char token) {
  return (token >= '0' && token <= '9') || (token >= 'a' && token <= 'f') ||
         (token >= 'A' && token <= 'F');
}
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
static constexpr bool IsCR(char token) { return token == 0x0D; }
static constexpr bool IsLF(char token) { return token == 0x0A; }
static constexpr bool IsHeadersEnd(types::camille::CamilleStringViewIt& pos,
                                   types::camille::CamilleStringViewIt end) {
  if ((end - pos) >= 2 && IsCR(*pos) && IsLF(*(pos + 1))) {
    pos += 2;
    if ((end - pos) >= 2 && IsCR(*pos) && IsLF(*(pos + 1))) {
      pos += 2;
      return false;
    }
  }
  return true;
}

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
    static constexpr std::array<char, 1> placements{' '};
  };
  struct AlphaNumericRequirement {
    static constexpr std::array<char, 62> placements{
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
  };
  struct HeaderKeySymbolRequirement {
    static constexpr std::array<char, 15> placements{'!', '#', '$', '%', '&', '\'', '*', '+',
                                                     '-', '.', '^', '_', '`', '|',  '~'};
  };
  struct HeaderValueSymbolRequirement {
    static constexpr auto placements = []() {
      std::array<char, 95> table{};
      for (size_t i = 0; i < 95; ++i) {
        table.at(i) = static_cast<char>(32 + i);
      }
      return table;
    }();
  };
  struct TabRequirement {
    static constexpr std::array<char, 1> placements{'\t'};
  };
  struct ReservedSymbolRequirement {
    static constexpr std::array<char, 4> placements{'#', '&', '=', '?'};
  };
  struct UnreservedSymbolRequirement {
    static constexpr std::array<char, 4> placements{'-', '.', '_', '~'};
  };
  struct BodyRequirement {
    static constexpr std::array<char, 1> placements{'0'};
  };

  using It = types::camille::CamilleStringViewIt;
  using UriTraits = ParserTraits<UnreservedSymbolRequirement, ReservedSymbolRequirement>;
  using HeaderKeyTraits = ParserTraits<AlphaNumericRequirement, HeaderKeySymbolRequirement>;
  using HeaderValueTraits = ParserTraits<HeaderValueSymbolRequirement, TabRequirement>;
  using BodyIdentifyTraits = ParserTraits<BodyRequirement>;
  using BodyChunkedTraits = ParserTraits<BodyRequirement>;

  struct Rocky {
    It begin;
    It end;
    std::string_view data;
  };

 public:
  Parser() = default;

  explicit operator bool() const { return current_state_ == States::kComplete; }

  void SetUsed() { used_ = true; }
  [[nodiscard]] bool IsDataEnd() const { return rocky_.begin == rocky_.end; }
  [[nodiscard]] std::uint8_t GetErrorCode() const { return static_cast<std::uint8_t>(error_); }
  [[nodiscard]] std::string_view GetErrorString() const { return error::ErrorToString(error_); }

  template <concepts::IsReqResType T>
  static bool ParseMethod(auto& pos, const It end, T& dtype) {
    auto begin = pos;
    while (pos != end && !IsSpace(*pos) && !IsLower(*pos)) {
      if (IsControl(*pos)) {
        return false;
      }
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
  static bool ParseUri(auto& pos, It end, T& dtype) {
    auto begin = pos;
    if (!IsSlash(*pos)) {
      return false;
    }
    ++pos;

    while (pos != end && !IsSpace(*pos)) {
      if (IsControl(*pos)) {
        return false;
      }
      if (!IsChar(*pos) && !UriTraits::IsValid(*pos) && !IsSlash(*pos)) {
        return false;
      }
      if (*pos == '%') {
        if ((pos + 2) >= end || !IsHexDigit(*(pos + 1)) || !IsHexDigit(*(pos + 2))) {
          return false;
        }
        pos += 2;
      }
      ++pos;
    }

    std::string_view path(begin, pos - begin);
    dtype.SetPath(path);
    return true;
  }

  template <concepts::IsReqResType T>
  static bool ParseVersion(auto& pos, const It end, T& dtype) {
    auto begin = pos;
    while (pos != end && !IsSlash(*pos)) {
      if (IsControl(*pos)) {
        return false;
      }
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
    while (pos != end && !IsCR(*pos)) {
      if (IsControl(*pos)) {
        return false;
      }
      if (!IsDigit(*pos) && *pos != '.') {
        return false;
      }
      ++pos;
    }

    if (pos != end && IsCR(*pos)) {
      ++pos;
    } else {
      return false;
    }

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
  static bool ParseHeaders(auto& pos, const It end, T& dtype) {
    It begin{};
    bool can_consume_more{true};

    while (can_consume_more && pos < end) {
      begin = pos;
      while (pos != end && *pos != ':') {
        if (HeaderKeyTraits::IsValid(*pos)) {
          ++pos;
        } else {
          return false;
        }
      }

      if (pos == begin || pos == end) {
        return false;
      }

      std::string_view current_key(begin, pos - begin);

      ++pos;
      if (pos != end && IsSpace(*pos)) {
        ++pos;
      } else {
        return false;
      }

      begin = pos;
      while (pos != end && !IsCR(*pos)) {
        if (HeaderValueTraits::IsValid(*pos)) {
          ++pos;
        } else {
          return false;
        }
      }

      auto owsit = pos;
      while (owsit > begin && IsSpace(*(owsit - 1))) {
        --owsit;
      }

      std::string_view current_value(begin, owsit - begin);
      dtype.AddHeader(current_key, current_value);
      if (current_key.size() == 4 && (current_key[0] | 0x20) == 'h' &&
          (current_key[1] | 0x20) == 'o' && (current_key[2] | 0x20) == 's' &&
          (current_key[3] | 0x20) == 't') {
        ParseHost(current_value, dtype);
      }

      begin = pos;

      if (!IsHeadersEnd(pos, end)) {
        can_consume_more = false;
      }
    }
    return true;
  }

  /**
   * @tparam T
   * @param pos
   * @param end
   * @param dtype
   * @param body_size - The size of the body (from content-length)
   * @param body_limit_ - Private member of the parser, used for post-validation with kBodyLimit
   */
  template <concepts::IsReqResType T>
  static bool ParseBodyIdentify(auto& pos, It end, T& dtype, std::string_view body_value) {
    // we still want a legit threshold.. therefore 10.
    if (body_value.size() > (std::numeric_limits<int>::max() / 10)) {
      return false;
    }

    size_t content_length{0};
    for (const auto& token : body_value) {
      if (!IsDigit(token)) {
        return false;
      }
      content_length = (content_length * 10) + (token - '0');
    }

    auto available = static_cast<size_t>(end - pos);
    if (content_length > kBodyLimit || available != content_length) {
      return false;
    }
    pos += available;

    return true;
  }

  template <concepts::IsReqResType T>
  static bool ParseBodyChunked(auto& pos, It end, T& dtype, std::string_view body_value) {
    return true;
  }

  template <concepts::IsReqResType T>
  [[nodiscard]] std::expected<T, error::Errors> Parse(std::string_view data) {
    if (data.empty()) {
      error_ = error::Errors::kBadRequest;
      return std::unexpected(error_);
    }

    T dtype;
    rocky_.begin = data.cbegin();
    rocky_.end = data.cend();
    rocky_.data = data;
    total_consumed_ = data.size();

    while (rocky_.begin != rocky_.end) {
      switch (current_state_) {
        case States::kReady:
          if (used_) {
            error_ = error::Errors::kStaleParser;
            current_state_ = States::kGarbage;
          }
          current_state_ = States::kMethod;
          break;

        case States::kMethod:
          if (!ParseMethod(rocky_.begin, rocky_.end, dtype)) {
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
          if (!ParseUri(rocky_.begin, rocky_.end, dtype)) {
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
          if (!ParseVersion(rocky_.begin, rocky_.end, dtype)) {
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
          if (ParseHeaders(rocky_.begin, rocky_.end, dtype)) {
            if (!dtype.GetHeader("Host").has_value()) {
              error_ = error::Errors::kBadRequest;
              current_state_ = States::kGarbage;
              break;
            }
            if (IsDataEnd()) {
              current_state_ = States::kComplete;
              dtype.SetSize(total_consumed_);
              return dtype;
              break;
            }
            if (dtype.GetHeader("Content-Length").has_value() &&
                dtype.GetHeader("Transfer-Encoding").has_value()) {
              error_ = error::Errors::kBadRequest;
              current_state_ = States::kGarbage;
            } else if (dtype.GetHeader("Content-Length").has_value()) {
              current_state_ = States::kBodyIdentify;
            } else if (dtype.GetHeader("Transfer-Encoding").has_value()) {
              current_state_ = States::kBodyChunked;
            } else {
              error_ = error::Errors::kBadRequest;
              current_state_ = States::kGarbage;
            }
          } else {
            error_ = error::Errors::kBadRequest;
            current_state_ = States::kGarbage;
          }
          break;

        case States::kBodyIdentify:
          if (!ParseBodyIdentify(rocky_.begin, rocky_.end, dtype,
                                 dtype.GetHeader("Content-Length").value())) {
            error_ = error::Errors::kBadBody;
            current_state_ = States::kGarbage;
          } else {
            if (IsDataEnd()) {
              current_state_ = States::kComplete;
              return dtype;
            }
            error_ = error::Errors::kBadBody;
            current_state_ = States::kGarbage;
          }
          break;

        case States::kBodyChunked:
          if (!ParseBodyChunked(rocky_.begin, rocky_.end, dtype,
                                dtype.GetHeader("Transfer-Encoding").value().size())) {
            error_ = error::Errors::kBadHeader;
            current_state_ = States::kGarbage;
          } else {
            if (IsDataEnd()) {
              current_state_ = States::kComplete;
              return dtype;
            }
            error_ = error::Errors::kBadBody;
            current_state_ = States::kGarbage;
          }
          break;

        case States::kComplete:
          SetUsed();
          if (!IsDataEnd()) {
            return std::unexpected(error::Errors::kStaleParser);
          }
          // dtype.SetSize(total_consumed_);
          // return dtype;
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
  States current_state_{States::kReady};
  error::Errors error_{error::Errors::kGeneralError};
};

};  // namespace parser
};  // namespace camille

#endif