#pragma once

namespace compass {

template <typename... Args>
struct PropFunc {
  bool (*func)(Args...);
};

} // namespace compass
