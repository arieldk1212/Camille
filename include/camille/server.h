#ifndef CAMILLE_INCLUDE_CAMILLE_SERVER_H_
#define CAMILLE_INCLUDE_CAMILLE_SERVER_H_

#include "stream.h"

#include "asio/ip/tcp.hpp"

namespace camille {
namespace server {

class Server {
 public:
  Server(std::string_view host, int port);

  void AsyncAccept(const asio::io_context& context, std::function<void> handler);

 private:
  int port_;
  std::string_view host_;
  std::atomic<bool> is_running_;
  std::shared_ptr<stream::Stream> stream_;
  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
};

};  // namespace server
};  // namespace camille

#endif