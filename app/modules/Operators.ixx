module;

#include <cstddef>

export module Operators;

namespace bits {
  export struct ShiftLeft {
    template <typename T>
    constexpr auto operator()(T lhs, std::size_t rhs) const -> T {
      return lhs << rhs;
    }
  };

  export struct ShiftRight {
    template <typename T>
    constexpr auto operator()(T lhs, std::size_t rhs) const -> T {
      return lhs >> rhs;
    }
  };

  export struct NoOp {
    template <typename T>
    constexpr auto operator()(T lhs, [[maybe_unused]] std::size_t _) const
        -> T {
      return lhs;
    }
  };
} // namespace bits
