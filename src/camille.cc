#include "camille/camille.h"

constexpr std::uint16_t port = 8085;
const unsigned pool = std::thread::hardware_concurrency();

int main() {
  camille::Camille client{};
  client.Run("127.0.0.1", port);
};