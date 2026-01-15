#ifndef CAMILLE_INCLUDE_CAMILLE_ERROR_H_
#define CAMILLE_INCLUDE_CAMILLE_ERROR_H_

#include <cstdint>

namespace camille {
namespace error {

enum class Errors : std::uint8_t {
  kEndOfStream,
  kSizeLimit,
  kBodyLimit,
  kBadRequest,
  kBufferOverflow,
  kPartialMessage,
  kShortRead,
  kStaleParser,  // Parser was already used (one parser per transaction).
  kGarbageRequest
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