#include "camille/camille.h"

int main() {
  camille::CamilleClient client;
  client.Run("0.0.0.0", 8085);
  return 0;
}