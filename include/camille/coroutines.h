#ifndef CAMILLE_INCLUDE_CAMILLE_COROUTINES_H_
#define CAMILLE_INCLUDE_CAMILLE_COROUTINES_H_

#include <coroutine>
#include <exception>

namespace camille {
namespace coroutine {

// struct promise_type {
//   T value;
//   Coroutine get_return_object() { return Coroutine{handle_type::from_promise(*this)}; }
//   std::initial_suspend_always initial_suspend() { return {}; }
//   std::final_suspend_always final_suspend() noexcept { return {}; }
//   void return_value(T v) { value = v; }
//   void unhandled_exception() { std::terminate(); }
// };

// template <typename T>
// class Coroutine {
//   using handle_type = std::coroutine_handle<promise_type>;
//   handle_type h;

//   Coroutine(handle_type h)
//       : h(h) {}
//   ~Coroutine() {
//     if (h) h.destroy();
//   }

//   T get() { return h.promise().value; }
// };

};  // namespace coroutine
};  // namespace camille

#endif