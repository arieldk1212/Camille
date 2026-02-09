#ifndef CAMILLE_INCLUDE_CAMILLE_ERROR_H_
#define CAMILLE_INCLUDE_CAMILLE_ERROR_H_

#include <cstdint>
#include <string_view>

namespace camille {
namespace error {

enum class Errors : std::uint8_t {
  kDefault,
  kBadMethod,
  kBadUri,
  kBadRequest,
  kStaleParser,
  kBadVersion,
  kBadKey,
  kBadHeader,
  kEndOfStream,
  kSizeLimit,
  kBodyLimit,
  kBadBody,
  kBadContentLength,
  kBufferOverflow,
  kPartialMessage,
  kShortRead,
  kGarbageRequest,
};

enum class NetworkError : std::uint8_t {
  kTimeout,
  kRemoteClosed,
  kConnectionReset,
  kQuotaExceeded
};

};  // namespace error
};  // namespace camille

#endif