#ifndef CAMILLE_INCLUDE_CAMILLE_LOGGING_H_
#define CAMILLE_INCLUDE_CAMILLE_LOGGING_H_

#include "types.h"

#include <print>
#include <chrono>
#include <format>

/**
 * @brief Camille Logging Functionality
 * @todo maybe just use spdlog async logger?
 * @todo add an option to write a test to file, follow benchmarks.
 */

namespace camille {
namespace logger {

static constexpr std::string_view TrimPath(std::string_view path) {
  /**
   * @brief Finding the shortest preferred human-read path.
   */
  using It = types::camille::CamilleStringViewIt;
  using CIt = const It;

  It last = path.cbegin();
  It second_last = path.cend();
  CIt end = path.cend();

  for (It pos{last}; pos != end; ++pos) {
    if (*pos == '/' || *pos == '\\') {
      second_last = last;
      last = pos + 1;
    }
  }

  if (second_last == path.cend()) {
    return path;
  }
  return path.substr(std::distance(path.cbegin(), second_last));
}

/**
 * @brief Core logging logic
 * @todo should maybe "downgrade" into cout?
 * @version 23
 * @param level
 * @param file
 * @param line
 * @param message
 * @tparam Args (args)
 */
template <typename... Args>
inline void InternalLog(std::string_view level,
                        std::string_view file,
                        int line,
                        std::format_string<Args...> message,
                        Args&&... args) {
  auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
  std::string formatted_message = std::format(message, std::forward<Args>(args)...);
  std::println("[{:%F %T} UTC] [{}] [{}:{}] {}", now, level, file, line, formatted_message);
  // std::cout << std::format("[{:%F %T} UTC] ", now) << "[" << level << "] "
  //           << "[" << file << ":" << line << "] " << formatted_message << "\n";
}

#define CAMILLE(message, ...)                                                                     \
  camille::logger::InternalLog("CAMILLE", camille::logger::TrimPath(__FILE__), __LINE__, message, \
                               ##__VA_ARGS__)
#define CAMILLE_TRACE(message, ...)                                                         \
  camille::logger::InternalLog("\033[32mTRACE\033[0m", camille::logger::TrimPath(__FILE__), \
                               __LINE__, message, ##__VA_ARGS__)
#define CAMILLE_DEBUG(message, ...)                                                         \
  camille::logger::InternalLog("\033[36mDEBUG\033[0m", camille::logger::TrimPath(__FILE__), \
                               __LINE__, message, ##__VA_ARGS__)
#define CAMILLE_INFO(message, ...)                                                             \
  camille::logger::InternalLog("INFO", camille::logger::TrimPath(__FILE__), __LINE__, message, \
                               ##__VA_ARGS__)
#define CAMILLE_WARNING(message, ...)                                                         \
  camille::logger::InternalLog("\033[33mWARNING\033[0m", camille::logger::TrimPath(__FILE__), \
                               __LINE__, message, ##__VA_ARGS__)
#define CAMILLE_ERROR(message, ...)                                                         \
  camille::logger::InternalLog("\033[31mERROR\033[0m", camille::logger::TrimPath(__FILE__), \
                               __LINE__, message, ##__VA_ARGS__)
#define CAMILLE_CRITICAL(message, ...)                                                         \
  camille::logger::InternalLog("\033[35mCRITICAL\033[0m", camille::logger::TrimPath(__FILE__), \
                               __LINE__, message, ##__VA_ARGS__)

};  // namespace logger
};  // namespace camille

#endif