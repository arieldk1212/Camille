#ifndef CAMILLE_INCLUDE_CAMILLE_CLIENT_H_
#define CAMILLE_INCLUDE_CAMILLE_CLIENT_H_

#include "middleware.h"
#include "router.h"
#include "logging.h"
#include "server.h"

#include <string>
#include <print>
#include <thread>

// TODO: think about how to set the debug mode, maybe in
// config file or something else, maybe not inside Camille?
namespace camille {

namespace client {

class BaseClient {
 public:
  virtual ~BaseClient() = default;

  // virtual void Run(const std::string& base_client_ip, int base_client_port) = 0;
  virtual void AddMiddleware(const middleware::BaseMiddleware& middleware) = 0;
  virtual void AddRouter(const router::Router& router) = 0;
};
};  // namespace client

class Camille : public client::BaseClient {
 public:
  Camille() { InitRouters(); }
  ~Camille() override {}

  Camille(const Camille&) = delete;
  Camille& operator=(const Camille&) = delete;

  Camille(Camille&&) = default;
  Camille& operator=(Camille&&) = default;

  void Run(const std::string& host, std::uint16_t port) {
    host_ = host;
    port_ = port;
    if (!server_) {
      server_ = std::make_unique<server::Server>(host_, port_, pool_size_);
    }
    server_->Run();
    CAMILLE("Listening at: http://{}:{}", host_, port_);
  }

  [[nodiscard]] bool IsDebugEnabled() const { return debug_; }

  void SetDebug(bool debug) { debug_ = debug; };
  void SetServerName(const std::string& server_name) { server_name_ = server_name; }
  void SetServerVersion(const std::string& server_version) { server_version_ = server_version; }
  void SetPoolSize(unsigned pool_size) { pool_size_ = pool_size; }

  void AddMiddleware(const middleware::BaseMiddleware& middleware) override {
    auto name = middleware.GetMiddlewareName();
    CAMILLE("Middleware Added {}", middleware.GetMiddlewareName());
  }
  void AddRouter(const router::Router& router) override {}

  /**
   * @brief usage in the ctor, appending and reading all
   *  the necessary components of the routers
   */
  constexpr void InitRouters() {
    if (!routers_.empty()) {
      for (const auto& router : routers_) {
        // TODO: probably add to swagger here or something.
      }
    }
  }

 private:
  bool debug_{false};
  std::string host_;
  std::uint16_t port_{0};
  std::string server_name_;
  std::string server_version_;
  std::vector<router::Router> routers_;  // maybe some sort of tree? prefix tree?
  types::camille::CamilleUnique<server::Server> server_;
  unsigned pool_size_{std::thread::hardware_concurrency()};
};

};  // namespace camille

#endif
