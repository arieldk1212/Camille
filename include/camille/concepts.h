#ifndef CAMILLE_INCLUDE_CAMILLE_CONCEPTS_H_
#define CAMILLE_INCLUDE_CAMILLE_CONCEPTS_H_

#include <concepts>
#include <string_view>

namespace camille {
namespace concepts {

/**
 * @brief For unsigned integers
 * @tparam T
 */
template <typename T>
concept UnsignedIntegral = std::unsigned_integral<T>;

/**
 * @brief For request/response classes
 * @tparam T
 * @param data
 */
template <typename T>
concept IsReqResType = requires(T val, std::string_view data) {
  {val.SetMethod(data)}->std::same_as<void>;
  {val.SetPath(data)}->std::same_as<void>;
  {val.SetVersion(data)}->std::same_as<void>;
  // {val.SetPort()}->std::same_as<void>;
  // {val.SetHost()}->std::same_as<void>;
};

};  // namespace concepts
};  // namespace camille

#endif
