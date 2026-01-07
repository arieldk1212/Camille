#ifndef CAMILLE_INCLUDE_CAMILLE_LOGGING_H_
#define CAMILLE_INCLUDE_CAMILLE_LOGGING_H_

#include <mutex>
#include <string>

// TODO: add timestamps.
namespace logger {

enum class LogLevels : std::uint8_t { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class Logging {
 public:
 private:
  std::string message_;
};

};  // namespace logger

#endif