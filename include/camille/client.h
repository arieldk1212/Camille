#ifndef CAMILLE_INCLUDE_CAMILLE_CLIENT_H_
#define CAMILLE_INCLUDE_CAMILLE_CLIENT_H_

#include "middleware.h"
#include "router.h"
#include "intermediary.h"

#include <string>

#define DEBUG(obj, msg)                              \
  do {                                               \
    if (obj) &&((obj)->IsDebugEnabled) {             \
        std::cout << "[DEBUG] " << msg << std::endl; \
      }                                              \
  } while (0)

// TODO: think about how to set the debug mode, maybe in
// config file or something else, maybe not inside Camille?
namespace camille {

namespace client {

class BaseClient {
 public:
  virtual ~BaseClient() = default;

  virtual void Run(const std::string& base_client_ip, int base_client_port) = 0;
  virtual void AddMiddleware(const camille::middleware::BaseMiddleware& middleware) = 0;
  virtual void AddRouter() = 0;
};
};  // namespace client

class Camille : public client::BaseClient {
 public:
  Camille() = default;
  ~Camille() override { server_->Stop(); }

  Camille(const Camille&) = delete;
  Camille& operator=(const Camille&) = delete;

  Camille(Camille&&) = default;
  Camille& operator=(Camille&&) = default;

  void Run(const std::string& client_ip, int client_port) override {
    intermediary::Run(server_, client_ip, client_port);
  }

  bool IsDebugEnabled() const { return debug_; }

  void SetDebug(bool debug) { debug_ = debug; };
  void SetServerName(const std::string& server_name) { server_name_ = server_name; }
  void SetServerVersion(const std::string& server_version) { server_version_ = server_version; }

  void AddMiddleware(const camille::middleware::BaseMiddleware& middleware) override {
    std::println("Middleware Added");
  }
  void AddRouter() override {}

  /**
   * @brief usage in the ctor, appending and reading all
   *  the necessary components of the routers
   *
   */
  constexpr void InitRouters();

 private:
  bool debug_{false};
  std::string server_name_;
  std::string server_version_;
  std::vector<router::Router> routers_;  // maybe some sort of tree? prefix tree?
  std::optional<server::Server> server_;
};

};  // namespace camille

#endif
