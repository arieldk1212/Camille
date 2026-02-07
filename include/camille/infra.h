#ifndef CAMILLE_INCLUDE_CAMILLE_INFRA_H_
#define CAMILLE_INCLUDE_CAMILLE_INFRA_H_

#include <string>
#include <cstdint>

namespace camille {
namespace infra {

namespace headers {
static constexpr std::string kConnection = "Connection";
static constexpr std::string kHost = "Host";
static constexpr std::string kContentLength = "Content-Length";
static constexpr std::string kContentType = "Content-Type";
static constexpr std::string kUserAgent = "User-Agent";
static constexpr std::string kSecCHUA = "Sec-CH-UA";
static constexpr std::string kAccept = "Accept";
static constexpr std::string kAcceptEncoding = "Accept-Encoding";
static constexpr std::string kAcceptLanguage = "Accept-Language";
static constexpr std::string kReferer = "Referer";
static constexpr std::string kAuthorization = "Authorization";
static constexpr std::string kCacheControl = "Cache-Control";
static constexpr std::string kCookie = "Cookie";
static constexpr std::string Date = "Date";
static constexpr std::string Expect = "Expect";
static constexpr std::string IfMatch = "If-Match";
static constexpr std::string IfNoneMatch = "If-None-Match";
// static constexpr std::string IfModifiedSince = "If-Modified-Since";
static constexpr std::string Origin = "Origin";
static constexpr std::string Range = "Range";
// static constexpr std::string TransferEncoding = "Transfer-Encoding";
static constexpr std::string Upgrade = "Upgrade";
static constexpr std::string XForwardedFor = "X-Forwarded-For";
};  // namespace headers

enum class Methods : std::uint8_t {
  kGet,
  kHead,
  kPost,
  kPatch,
  kPut,
  kDelete,

  kOptions,
  kConnect,
  kTrace,
  kUnknown
};

static constexpr Methods MethodEnum(std::string_view method) {
  /**
   * @brief Cannot switch-case on string_view
   */
  if (method == "GET") {
    return Methods::kGet;
  }
  if (method == "POST") {
    return Methods::kPost;
  }
  if (method == "PUT") {
    return Methods::kPut;
  }
  if (method == "DELETE") {
    return Methods::kDelete;
  }
  if (method == "PATCH") {
    return Methods::kPatch;
  }
  if (method == "HEAD") {
    return Methods::kHead;
  }
  if (method == "OPTIONS") {
    return Methods::kOptions;
  }
  if (method == "TRACE") {
    return Methods::kTrace;
  }
  if (method == "CONNECT") {
    return Methods::kConnect;
  }
  return Methods::kUnknown;
}

// TODO: need to fill the rest, change the name to the error itself
enum class StatusCodes : std::uint16_t {
  HTTP_100 = 100,
  HTTP_200 = 200,
  HTTP_300 = 300,
  HTTP_400 = 400,
  HTTP_500 = 500
};

};  // namespace infra
};  // namespace camille

#endif