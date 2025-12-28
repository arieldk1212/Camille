#include "camille/camille.h"

#include <memory>
#include "camille/client.h"

int main() {
  std::shared_ptr<camille::CamilleClient> client = std::make_shared<camille::CamilleClient>();
  client->Run("0.0.0.0", 8085);
  return 0;
}