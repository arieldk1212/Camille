#ifndef CAMILLE_INCLUDE_CAMILLE_STREAM_H_
#define CAMILLE_INCLUDE_CAMILLE_STREAM_H_

#include "types.h"

#include "asio/io_context.hpp"

namespace camille {
namespace stream {

class Stream {
 public:
  explicit Stream(size_t stream_pool_size)
      : stream_pool_size(stream_pool_size) {}

  void IoContextRun() {
    /**
     * @todo implement which runs via round robin or lru?
     */
  }

 private:
  size_t stream_pool_size;
  std::vector<asio::io_context> io_contexts_;
  std::vector<types::aio::SharedAsioWorkGuards> work_guards_;
};

};  // namespace stream
};  // namespace camille

#endif