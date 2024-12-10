#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <type_traits>

namespace contracts {
  template <typename Tp>
  concept IsArithmetic = std::is_arithmetic_v<Tp>;
  template <typename Tp>
  concept IsIntegral = std::is_integral_v<Tp>;
  template <typename Tp>
  concept IsFloatingPoint = std::is_floating_point_v<Tp>;
} // namespace contracts

#endif
