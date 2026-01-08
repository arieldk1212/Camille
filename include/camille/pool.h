#ifndef CAMILLE_INCLUDE_CAMILLE_POOL_H_
#define CAMILLE_INCLUDE_CAMILLE_POOL_H_

#include "types.h"
#include "concepts.h"
#include "logging.h"

#include <thread>

namespace camille {
namespace pool {

class Pool {
 public:
  virtual ~Pool() = default;
};

class ContextPool : public Pool {
 public:
  explicit ContextPool(
      concepts::UnsignedIntegral auto pool_size = std::thread::hardware_concurrency())
      : pool_size_(pool_size) {
    for (size_t i{0}; i < pool_size_; ++i) {
      auto ctx = std::make_shared<types::aio::AsioIOContext>();
      auto work_guard = std::make_shared<asio::executor_work_guard<types::aio::AsioExecutorType>>(
          asio::make_work_guard(*ctx));
      io_contexts_.emplace_back(ctx);
      work_guards_.emplace_back(work_guard);
    }
  }

  void Run() {
    for (const auto& ctx : io_contexts_) {
      threads_.emplace_back([&ctx]() { ctx->run(); });
      CAMILLE("BOOTING WORKER");
    }
  }

  types::aio::AsioIOContext& GetIOContext() {
    size_t index = next_io_context_.fetch_add(1, std::memory_order_relaxed) % io_contexts_.size();
    return *io_contexts_[index];
  }

 private:
  unsigned pool_size_;
  std::atomic<size_t> next_io_context_{0};
  types::aio::SharedAsioIoContextVector io_contexts_;
  types::aio::SharedAsioWorkGuardsVector work_guards_;
  types::camille::CamilleVector<std::jthread> threads_;
};

};  // namespace pool
};  // namespace camille

#endif