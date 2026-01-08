#ifndef CAMILLE_INCLUDE_CAMILLE_MIDDLEWARE_H_
#define CAMILLE_INCLUDE_CAMILLE_MIDDLEWARE_H_

#include <string>

/**
 * @brief implement a proxy for the middlewares.
 */

namespace camille {
namespace middleware {

class BaseMiddleware {
 public:
  virtual ~BaseMiddleware() = default;

  [[nodiscard]] virtual std::string GetMiddlewareName() const = 0;
};

class HTTPMiddleware : public BaseMiddleware {
 public:
  HTTPMiddleware();

  [[nodiscard]] std::string GetMiddlewareName() const override { return middlware_name_; }

 private:
  std::string middlware_name_{"HTTP"};
};

};  // namespace middleware
};  // namespace camille

#endif