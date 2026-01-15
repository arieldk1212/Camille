#ifndef CAMILLE_INCLUDE_CAMILLE_LOGGING_H_
#define CAMILLE_INCLUDE_CAMILLE_LOGGING_H_

#include "types.h"

#include <print>
#include <chrono>
#include <format>
/**
 * @brief Camille Logging Functionality
 * @todo maybe just use spdlog async logger?
 */

namespace camille {

namespace logger {

constexpr std::string_view TrimPath(std::string_view path) {
  /**
   * @brief Finding the shortest preferred human-read path.
   * @todo test it, benchmark, fix frozen terminal..
   */
  using It = std::string_view::const_iterator;

  It last = path.cbegin();
  It second_last = path.cend();
  const It end = path.cend();

  for (It pos = last; pos != end; ++pos) {
    if (*pos == '/' || *pos == '\\') {
      second_last = last;
      last = pos + 1;
    }
  }

  if (second_last == path.cend()) {
    return path;
  }

  return path.substr(std::distance(path.cbegin(), second_last));

  // size_t last_slash = path.find_last_of("/\\");
  // if (last_slash == std::string_view::npos) return path;

  // // Logic to get the "second to last" part if that's your goal:
  // std::string_view prefix = path.substr(0, last_slash);
  // size_t second_last_slash = prefix.find_last_of("/\\");

  // if (second_last_slash == std::string_view::npos) return path;
  // return path.substr(second_last_slash + 1);
}

/**
 * @brief Core logging logic, version 23 only
 * @todo should maybe "downgrade" into cout?
 * @tparam Args
 * @param level
 * @param file
 * @param line
 * @param message
 * @param args
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