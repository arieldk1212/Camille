#ifndef CAMILLE_INCLUDE_CAMILLE_LOGGING_H_
#define CAMILLE_INCLUDE_CAMILLE_LOGGING_H_

#include <print>
#include <chrono>
#include <iostream>
#include <format>

/**
 * @brief Camille Logging Functionality
 * @todo maybe just use spdlog async logger?
 */

namespace logger {

constexpr const char* TrimPath(const char* path) {
  /**
   * @brief sliding window algo for finding the shortened path
   */
  const char* last = path;
  const char* second_last = path;
  for (const char* pos = path; *pos; ++pos) {
    if (*pos == '/' || *pos == '\\') {
      second_last = last;
      last = pos + 1;
    }
  }
  return (second_last == path) ? last : second_last;
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

#define CAMILLE(message, ...) \
  logger::InternalLog("CAMILLE", logger::TrimPath(__FILE__), __LINE__, message, ##__VA_ARGS__)
#define CAMILLE_TRACE(message, ...)                                                          \
  logger::InternalLog("\033[32mTRACE\033[0m", logger::TrimPath(__FILE__), __LINE__, message, \
                      ##__VA_ARGS__)
#define CAMILLE_DEBUG(message, ...)                                                          \
  logger::InternalLog("\033[36mDEBUG\033[0m", logger::TrimPath(__FILE__), __LINE__, message, \
                      ##__VA_ARGS__)
#define CAMILLE_INFO(message, ...) \
  logger::InternalLog("INFO", logger::TrimPath(__FILE__), __LINE__, message, ##__VA_ARGS__)
#define CAMILLE_WARNING(message, ...)                                                          \
  logger::InternalLog("\033[33mWARNING\033[0m", logger::TrimPath(__FILE__), __LINE__, message, \
                      ##__VA_ARGS__)
#define CAMILLE_ERROR(message, ...)                                                          \
  logger::InternalLog("\033[31mERROR\033[0m", logger::TrimPath(__FILE__), __LINE__, message, \
                      ##__VA_ARGS__)
#define CAMILLE_CRITICAL(message, ...)                                                          \
  logger::InternalLog("\033[35mCRITICAL\033[0m", logger::TrimPath(__FILE__), __LINE__, message, \
                      ##__VA_ARGS__)

};  // namespace logger

#endif