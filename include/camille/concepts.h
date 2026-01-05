#ifndef CAMILLE_INCLUDE_CAMILLE_CONCEPTS_H_
#define CAMILLE_INCLUDE_CAMILLE_CONCEPTS_H_

#include <concepts>
#include <string>
#include <type_traits>

namespace camille {
namespace concepts {

template <typename T>
concept IsRequest = requires(T r) {
  {r.path()}->std::convertible_to<std::string_view>;
  {r.method()}->std::convertible_to<std::string_view>;
};

template <typename F, typename Req>
concept WebHandler = requires(F f, Req r) {
  f(r);
};

};  // namespace concepts
};  // namespace camille

#endif
