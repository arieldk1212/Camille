#include "camille/camille.h"

#include <memory>

using Server = camille::Camille;
using Router = camille::router::Router;

int main() {
  std::shared_ptr<Server> camille = std::make_shared<Server>();
  camille->SetDebug(true);
  camille->Run("0.0.0.0", 8085);

  return 0;
}