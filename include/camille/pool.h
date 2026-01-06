#ifndef CAMILLE_INCLUDE_CAMILLE_POOL_H_
#define CAMILLE_INCLUDE_CAMILLE_POOL_H_

#include "types.h"
#include "concepts.h"

#include <thread>

namespace camille {
namespace pool {

class Pool {
 public:
  virtual ~Pool() = default;
};

class ContextPool {
 public:
  explicit ContextPool(concepts::SignedIntegral auto pool_size)
      : pool_size_(pool_size) {
    for (size_t i{0}; i < pool_size_; ++i) {
      auto ctx = std::make_shared<types::aio::AsioIOContext>();
      auto work_guard = asio::make_work_guard(*ctx);
      io_contexts_.emplace_back(ctx);
      work_guards_.emplace_back(&work_guard);
    }
  }

  void run() {
    types::camille::CamilleVector<std::thread> threads;
    for (const auto& ctx : io_contexts_) {
      threads.emplace_back([&ctx]() { ctx->run(); });
    }
    for (auto& ctx : threads) {
      if (ctx.joinable()) {
        ctx.join();
      }
    }
  }

  types::aio::AsioIOContext& get_io_context() {
    auto& ctx = *io_contexts_[next_io_context_];
    next_io_context_ = (next_io_context_ + 1) % io_contexts_.size();
    return ctx;
  }

 private:
  unsigned pool_size_{std::thread::hardware_concurrency()};
  size_t next_io_context_{0};
  types::aio::SharedAsioWorkGuardsVector work_guards_;
  types::aio::SharedAsioIoContextVector io_contexts_;
};

};  // namespace pool
};  // namespace camille

#endif