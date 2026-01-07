#ifndef CAMILLE_INCLUDE_CAMILLE_BENCHMARK_H_
#define CAMILLE_INCLUDE_CAMILLE_BENCHMARK_H_

#include <chrono>
#include <print>

namespace camille {

class Benchmark {
 public:
  explicit Benchmark(std::string benchmark_name)
      : benchmark_name_(std::move(benchmark_name)),
        start_(std::chrono::high_resolution_clock::now()) {}
  ~Benchmark() {
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);

    std::println("Benchmarked At: {}ms", elapsed_seconds);
  }

 private:
  std::string benchmark_name_;
  std::chrono::high_resolution_clock::time_point start_;
};

};  // namespace camille

#endif