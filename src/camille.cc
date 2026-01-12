#include "camille/camille.h"

constexpr std::uint16_t port = 8085;
const unsigned pool = std::thread::hardware_concurrency();

int main() {
  camille::Camille client{};
  /**
   * @todo fix this debug seg fault error, probably do need DI.
   */
  // client.SetDebug(true);
  client.Run("127.0.0.1", port);
};