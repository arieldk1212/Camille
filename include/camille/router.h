#ifndef CAMILLE_INCLUDE_CAMILLE_ROUTER_H_
#define CAMILLE_INCLUDE_CAMILLE_ROUTER_H_

#include <string>
#include <optional>

#include "infra.h"

namespace camille {
namespace router {

class Router {
 public:
  Router(const std::string& prefix, std::optional<std::string>& tag);
  ~Router() = default;

  void Head(const std::string& path, infra::StatusCodes status_code);
  void Get(const std::string& path, infra::StatusCodes status_code);
  void Post(const std::string& path, infra::StatusCodes status_code);
  void Patch(const std::string& path, infra::StatusCodes status_code);
  void Put(const std::string& path, infra::StatusCodes status_code);
  void Options(const std::string& path, infra::StatusCodes status_code);
  void Delete(const std::string& path, infra::StatusCodes status_code);

  /**
   * @brief Acts as a wrapper (aka python decorator)
   * @tparam MethodType - for example, get
   * @tparam Args - the args that the user passes to the method
   * @param method
   */
  template <typename MethodType, typename... Args>
  void ToMacro(const MethodType& method);

 private:
  // std::shared_ptr<server::Client> server_;
};

};  // namespace router
};  // namespace camille

#endif