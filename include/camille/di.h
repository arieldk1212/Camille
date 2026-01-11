#ifndef CAMILLE_INCLUDE_CAMILLE_DEPENDENCY_INJECTION_H_
#define CAMILLE_INCLUDE_CAMILLE_DEPENDENCY_INJECTION_H_

class IDI {
 public:
  virtual ~IDI() = default;

  [[nodiscard]] virtual IDI& GetService() const = 0;
};

class ServerDI : public IDI {
 public:
  ServerDI();

  void SetServerState();

 private:
  bool server_state_;  // debug or not
};

#endif