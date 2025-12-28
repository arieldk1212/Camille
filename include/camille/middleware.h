#ifndef CAMILLE_INCLUDE_CAMILLE_MIDDLEWARE_H_
#define CAMILLE_INCLUDE_CAMILLE_MIDDLEWARE_H_

namespace camille {
namespace middleware {
class BaseMiddleware {
 public:
  virtual ~BaseMiddleware() = default;
};

};  // namespace middleware
};  // namespace camille

#endif