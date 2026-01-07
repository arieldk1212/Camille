#ifndef CAMILLE_INCLUDE_CAMILLE_MIDDLEWARE_H_
#define CAMILLE_INCLUDE_CAMILLE_MIDDLEWARE_H_

/**
 * @brief implement a proxy for the middlewares.
 */

namespace camille {
namespace middleware {
class BaseMiddleware {
 public:
  virtual ~BaseMiddleware() = default;
};

};  // namespace middleware
};  // namespace camille

#endif