#ifndef CAMILLE_INCLUDE_CAMILLE_CONCEPTS_H_
#define CAMILLE_INCLUDE_CAMILLE_CONCEPTS_H_

#include <concepts>
#include <string_view>
#include <span>
#include <optional>

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
concept IsReqResType = requires(T val, std::string_view data, size_t size) {
  {val.SetHost(data)}->std::same_as<void>;
  {val.SetPort(data)}->std::same_as<void>;
  {val.SetPath(data)}->std::same_as<void>;
  {val.SetMethod(data)}->std::same_as<void>;
  {val.SetVersion(data)}->std::same_as<void>;
  {val.AddHeader(data, data)}->std::same_as<void>;
  {val.GetHeader(data)}->std::same_as<std::optional<std::string_view>>;
  {val.SetSize(size)}->std::same_as<void>;
};

template <typename T>
concept SymbolRequirement = requires() {
  {T::placements}->std::convertible_to<std::span<const char>>;
};

};  // namespace concepts
};  // namespace camille

#endif
