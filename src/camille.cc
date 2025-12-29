#include "camille/camille.h"

#include <memory>

using Server = camille::Camille;
using Router = camille::router::Router;

int main() {
  std::shared_ptr<Server> camille = std::make_shared<Server>();
  camille->SetDebug(false);  // Verify: should debug be enabled?

  try {
    camille->Run("127.0.0.1", 8085);
  } catch (const std::exception& e) {
    std::println("Error: {}", e.what());
    return 1;
  }

  return 0;
}