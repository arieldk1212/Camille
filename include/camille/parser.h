#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "types.h"

#include <cstdint>
#include <string_view>

namespace camille {
namespace parser {

class Parser {
 public:
  Parser(size_t limit, types::camille::CamilleHeaders& headers)
      : data_limit_(limit),
        headers_(headers) {}
  ~Parser() { current_state_ = States::finish; }

  enum class States : std::uint8_t { ready, wait, head, headers, body, finish };
  std::uint64_t body_limit = 64 * 1024;

  bool operator()() const { return current_state_ == States::finish; }

  void SetState(States state) { current_state_ = state; }

  States StateMachine(auto prim) {
    if (prim == "\n") {
      current_state_ = States::head;
    }
    return current_state_;
  }

  void ParseHeader();
  void ParseHead();
  void ParseFinish();
  void ParseBody();
  void WriteHeaders();

  void ParseAll(std::string_view data) {
    SetState(States::ready);
    std::string curr;
    for (int i{0}; i < sizeof(data_); ++i) {
    }
  }

 private:
  types::camille::CamilleHeaders headers_;
  const char* data_;
  size_t data_limit_;
  States current_state_{States::ready};
};

};  // namespace parser
};  // namespace camille

#endif