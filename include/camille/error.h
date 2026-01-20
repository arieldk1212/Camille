#ifndef CAMILLE_INCLUDE_CAMILLE_ERROR_H_
#define CAMILLE_INCLUDE_CAMILLE_ERROR_H_

#include <cstdint>

namespace camille {
namespace error {

enum class Errors : std::uint8_t {
  kGeneralError,
  kBadMethod,
  kBadUri,
  kBadRequest,
  kStaleParser,  // Parser was already used (one parser per transaction).
  kBadVersion,   // could be due to mismatch between applications or wrong version.
  kBadKey,
  kBadHeader,
  kEndOfStream,
  kSizeLimit,
  kBodyLimit,
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