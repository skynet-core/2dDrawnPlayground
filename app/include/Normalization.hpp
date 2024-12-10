#ifndef NORMALIZATION_HPP
#define NORMALIZATION_HPP

#include "Contracts.hpp"
#include <cmath>
#include <iterator>
#include <limits>

namespace color {
  using namespace contracts;

  constexpr auto NUMBITS = 8;

  template <IsFloatingPoint Tp, IsIntegral Up>
  constexpr auto normalize(Up value) noexcept -> Tp {
    return static_cast<Tp>(value) / std::numeric_limits<Up>::max();
  }

  template <IsIntegral Tp, IsFloatingPoint Up>
  constexpr auto denormalize(Up value) noexcept -> Tp {
    return static_cast<Tp>(std::round(value * std::numeric_limits<Tp>::max()));
  }

  // template <IsIntegral Rt, IsIntegral It>
  //   requires(sizeof(Rt) > sizeof(It))
  // constexpr auto scale(It value) noexcept -> Rt {
  //   return static_cast<Rt>(static_cast<double>(value) /
  //                          (1 << sizeof(It) * NUMBITS) *
  //                          (1 << sizeof(Rt) * NUMBITS)) -
  //          value;
  // }

  // template <IsIntegral Rt, IsIntegral It>
  //   requires(sizeof(Rt) < sizeof(It))
  // constexpr auto scale(It value) noexcept -> Rt {
  //   return static_cast<Rt>(
  //       std::round(static_cast<double>(value) /
  //                  (1 << NUMBITS * (sizeof(It) - sizeof(Rt)))));
  // }

  template <IsIntegral Rt, IsIntegral It>
    requires(sizeof(Rt) > sizeof(It))
  constexpr auto scale(It value) noexcept -> Rt {
    return static_cast<Rt>(value) << (NUMBITS * (sizeof(Rt) - sizeof(It)));
  }

  template <IsIntegral Rt, IsIntegral It>
    requires(sizeof(Rt) < sizeof(It))
  constexpr auto scale(It value) noexcept -> Rt {
    return static_cast<Rt>(value >> (NUMBITS * (sizeof(It) - sizeof(Rt))));
  }

  template <IsIntegral Rt, IsIntegral It>
    requires(sizeof(Rt) == sizeof(It))
  constexpr auto scale(It value) noexcept -> Rt {
    return value;
  }

  template <IsIntegral Rt, IsFloatingPoint It>
  constexpr auto cast(It value) noexcept -> Rt {
    return denormalize<Rt>(value);
  }

  template <IsFloatingPoint Rt, IsIntegral It>
  constexpr auto cast(It value) noexcept -> Rt {
    return normalize<Rt>(value);
  }

  template <IsIntegral Rt, IsIntegral It>
  constexpr auto cast(It value) noexcept -> Rt {
    return scale<Rt>(value);
  }

} // namespace color

#endif