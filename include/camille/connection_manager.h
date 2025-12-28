#ifndef CAMILLE_INCLUDE_CAMILLE_CONNECTION_H_
#define CAMILLE_INCLUDE_CAMILLE_CONNECTION_H_

#include <mutex>
#include <atomic>

namespace camille {

// Connection manager for handling multiple concurrent connections
// (cpp-httplib style: could be extended with thread pool)
class ConnectionManager {
 public:
  ConnectionManager()
      : connection_count_(0) {}

  void increment() { ++connection_count_; }
  void decrement() { --connection_count_; }
  [[nodiscard]] int count() const { return connection_count_; }

 private:
  std::mutex connection_mutex_;
  std::atomic<int> connection_count_;
};

}  // namespace camille

#endif