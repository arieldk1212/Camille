#ifndef CAMILLE_INCLUDE_CAMILLE_INFRA_H_
#define CAMILLE_INCLUDE_CAMILLE_INFRA_H_

#include "types.h"

namespace camille {

namespace infra {

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

static const types::camille::CamilleUnorderedMap<Methods, std::string_view> MethodRep{
    {Methods::kGet, "GET"},         {Methods::kHead, "HEAD"},       {Methods::kPost, "POST"},
    {Methods::kPatch, "PATCH"},     {Methods::kPut, "PUT"},         {Methods::kDelete, "DELETE"},
    {Methods::kOptions, "OPTIONS"}, {Methods::kConnect, "CONNECT"}, {Methods::kTrace, "TRACE"},
    {Methods::kUnknown, "UNKNOWN"},
};

static constexpr std::string_view MethodString(Methods method) { return MethodRep.at(method); }

static constexpr Methods MethodEnum(std::string_view method) {
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