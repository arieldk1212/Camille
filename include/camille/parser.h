#ifndef CAMILLE_INCLUDE_CAMILLE_PARSER_H_
#define CAMILLE_INCLUDE_CAMILLE_PARSER_H_

#include "types.h"

#include <cstdint>
#include <string_view>

namespace camille {
namespace parser {

constexpr static std::uint64_t kBodyLimit = 64 * 1024;
enum class States : std::uint8_t {
  READY,
  WAIT,
  METHOD,
  URI,
  VERSION,
  KEY,
  VALUE,
  BODY,
  COMPLETE,
  GARBAGE
};

/**
 * @ref request smuggling
 * always add body and be willing to accept one.
 * interact from front and back with the same http version.
 * throw error on exception, dont continue.
 */

class Parser {
 public:
  explicit Parser(std::string_view data)
      : data_(data) {}

  explicit operator bool() const { return current_state_ == States::COMPLETE; }

  void SetState(States state) { current_state_ = state; }

  States StateMachine(auto prim) {
    if (prim == "\n") {
      // current_state_ = States::head;
    }
    return current_state_;
  }

  void ParseHead();
  void ParseBody();
  void ParseHeader();
  void WriteHeaders();
  void ParseAll(std::string_view data) {
    SetState(States::METHOD);

    switch(current_state_) {
      case (States::METHOD):
        
    }
  }

 private:
  std::string_view data_;                   // the request data
  size_t data_limit_{kBodyLimit};           // total limit of the request
  States current_state_{States::READY};     // current state of the machine
  types::camille::CamilleHeaders headers_;  // headers
  // types::aio::AsioIOMutableBuffer buffer_;
};

};  // namespace parser
};  // namespace camille

#endif