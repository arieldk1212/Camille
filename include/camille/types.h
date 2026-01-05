#ifndef CAMILLE_INCLUDE_CAMILLE_TYPES_H_
#define CAMILLE_INCLUDE_CAMILLE_TYPES_H_

#include "asio/io_context.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace camille {
namespace types {

namespace camille {
/**
 * @brief Camille Shared Pointer
 * @tparam SharedPtrType
 */
template <typename SharedPtrType>
using CamilleShared = std::shared_ptr<SharedPtrType>;

/**
 * @brief Camille Unique Pointer
 * @tparam UniquePtrType
 */
template <typename UniquePtrType>
using CamilleUnique = std::shared_ptr<UniquePtrType>;

/**
 * @brief Headers data structure
 */
using CamilleHeaders = std::unordered_map<std::string, std::string>;
};  // namespace camille

namespace http {
/**
 * @brief HTTP Parser data strcuture
 */
using ParserData = std::vector<std::tuple<std::string, std::string, std::string>>;
};  // namespace http

namespace aio {
/**
 * @brief Asio Executor Type
 */
using AsioExecutorType = asio::io_context::executor_type;

/**
 * @brief Shared ptr of Asio work guards
 */
using SharedAsioWorkGuards = camille::CamilleShared<asio::executor_work_guard<AsioExecutorType>>;
};  // namespace aio

};  // namespace types
};  // namespace camille

#endif