#ifndef CAMILLE_INCLUDE_CAMILLE_SERVER_H_
#define CAMILLE_INCLUDE_CAMILLE_SERVER_H_

#include "pool.h"

#include <string_view>

namespace camille {
namespace server {

class Server {
 public:
  Server(std::string_view host, std::uint16_t port, concepts::SignedIntegral auto pool_siz) {}

 private:
  pool::ContextPool io_context_pool_;
  types::aio::AsioIOAcceptor acceptor_;
};

};  // namespace server
};  // namespace camille

#endif