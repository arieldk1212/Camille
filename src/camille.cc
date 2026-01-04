#include "camille/camille.h"

#include <memory>

using Server = camille::Camille;
using Router = camille::router::Router;

std::string helper(const camille::request::Request& req) { return "Hello!"; }

int main() {
  std::shared_ptr<Server> camille = std::make_shared<Server>();
  camille->SetDebug(false);

  // Router main_router{"/", "main"};

  // main_router.Get("/", camille::infra::StatusCodes::HTTP_200);

  // camille->AddRouter(main_router);

  try {
    camille->Run("127.0.0.1", 8085);
  } catch (const std::exception& e) {
    std::println("Error: {}", e.what());
    return 1;
  }

  return 0;
}