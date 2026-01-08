#ifndef CAMILLE_INCLUDE_CAMILLE_TYPES_H_
#define CAMILLE_INCLUDE_CAMILLE_TYPES_H_

#include "asio/io_context.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/streambuf.hpp"

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
using CamilleUnique = std::unique_ptr<UniquePtrType>;

/**
 * @brief Camille Vector
 * @tparam VectorType
 */
template <typename VectorType>
using CamilleVector = std::vector<VectorType>;

/**
 * @brief Headers data structure
 */
using CamilleHeaders = std::unordered_map<std::string, std::string>;
};  // namespace camille

namespace http {
/**
 * @brief HTTP Parser data structure
 * @details Tuple contains (method, path, version) or document what each element represents
 */
using ParserData = std::vector<std::tuple<std::string, std::string, std::string>>;
};  // namespace http

namespace aio {
/**
 * @brief Asio Io Context Executor Type
 */
using AsioExecutorType = asio::io_context::executor_type;

/**
 * @brief shared_ptr of Asio work guards
 */
using SharedAsioWorkGuards = camille::CamilleShared<asio::executor_work_guard<AsioExecutorType>>;

/**
 * @brief Asio IO Context
 */
using AsioIOContext = asio::io_context;

/**
 * @brief Asio Socket
 */
using AsioIOSocket = asio::ip::tcp::socket;

/**
 * @brief Asio Acceptor
 */
using AsioIOAcceptor = asio::ip::tcp::acceptor;

/**
 * @brief Asio Stream Buffer
 */
using AsioIOStreamBuffer = asio::streambuf;

/**
 * @brief Asio IP Address
 */
using AsioIOAddress = asio::ip::address;

/**
 * @brief vector shared_ptr of Asio io context
 */
using SharedAsioIoContextVector =
    camille::CamilleVector<camille::CamilleShared<aio::AsioIOContext>>;

/**
 * @brief vector of shared_ptr Asio work guards
 */
using SharedAsioWorkGuardsVector = camille::CamilleVector<aio::SharedAsioWorkGuards>;

};  // namespace aio

};  // namespace types
};  // namespace camille

#endif