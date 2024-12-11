module;

#include <type_traits>

export module Contracts;

namespace contracts {
  export template <typename Tp>
  concept IsArithmetic = std::is_arithmetic_v<Tp>;

  export template <typename Tp>
  concept IsIntegral = std::is_integral_v<Tp>;

  export template <typename Tp>
  concept IsFloatingPoint = std::is_floating_point_v<Tp>;
} // namespace contracts