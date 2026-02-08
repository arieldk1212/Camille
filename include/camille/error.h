#ifndef CAMILLE_INCLUDE_CAMILLE_ERROR_H_
#define CAMILLE_INCLUDE_CAMILLE_ERROR_H_

#include <cstdint>
#include <string_view>

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

static constexpr std::string_view ErrorToString(const Errors error) {
  switch (error) {
    case Errors::kGeneralError:
      return "General Error";
    case Errors::kBadMethod:
      return "Bad Method";
    case Errors::kBadUri:
      return "Bad URI";
    case Errors::kBadRequest:
      return "Bad Request";
    case Errors::kStaleParser:
      return "Stale Parser: One transaction per parser";
    case Errors::kBadVersion:
      return "Bad Version: Protocol mismatch or invalid format";
    case Errors::kBadKey:
      return "Bad Key";
    case Errors::kBadHeader:
      return "Bad Header";
    case Errors::kEndOfStream:
      return "End of Stream reached unexpectedly";
    case Errors::kSizeLimit:
      return "Header Size Limit Exceeded";
    case Errors::kBodyLimit:
      return "Body Size Limit Exceeded";
    case Errors::kBufferOverflow:
      return "Internal Buffer Overflow";
    case Errors::kPartialMessage:
      return "Partial Message: More data expected";
    case Errors::kShortRead:
      return "Short Read from Network";
    case Errors::kGarbageRequest:
      return "Garbage Request: Unparseable data";
    default:
      return "Unknown Error";
  }
}

static constexpr std::string_view ErrorToString(const NetworkError error) {
  switch (error) {
    case NetworkError::kTimeout:
      return "Network Timeout";
    case NetworkError::kRemoteClosed:
      return "Remote Host Closed Connection";
    case NetworkError::kConnectionReset:
      return "Connection Reset by Peer";
    case NetworkError::kQuotaExceeded:
      return "Network Quota Exceeded";
    default:
      return "Unknown Network Error";
  }
}

};  // namespace error
};  // namespace camille

#endif