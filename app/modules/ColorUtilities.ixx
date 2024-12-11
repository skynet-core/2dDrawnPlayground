module;

#include <climits>
#include <cmath>
#include <limits>

export module ColorUtilities;

import Contracts;

namespace color {
  using namespace contracts;

  export template <IsFloatingPoint Tp, IsIntegral Up>
  constexpr auto normalize(Up value) noexcept -> Tp {
    return static_cast<Tp>(value) / std::numeric_limits<Up>::max();
  }

  export template <IsIntegral Tp, IsFloatingPoint Up>
  constexpr auto denormalize(Up value) noexcept -> Tp {
    return static_cast<Tp>(std::round(value * std::numeric_limits<Tp>::max()));
  }

  export template <IsIntegral Rt, IsIntegral It>
    requires(sizeof(Rt) > sizeof(It))
  constexpr auto scale(It value) noexcept -> Rt {
    return static_cast<Rt>(value) << (CHAR_BIT * (sizeof(Rt) - sizeof(It)));
  }

  export template <IsIntegral Rt, IsIntegral It>
    requires(sizeof(Rt) < sizeof(It))
  constexpr auto scale(It value) noexcept -> Rt {
    return static_cast<Rt>(value >> (CHAR_BIT * (sizeof(It) - sizeof(Rt))));
  }

  export template <IsIntegral Rt, IsIntegral It>
    requires(sizeof(Rt) == sizeof(It))
  constexpr auto scale(It value) noexcept -> Rt {
    return value;
  }

  export template <IsIntegral Rt, IsFloatingPoint It>
  constexpr auto cast(It value) noexcept -> Rt {
    return denormalize<Rt>(value);
  }

  export template <IsFloatingPoint Rt, IsIntegral It>
  constexpr auto cast(It value) noexcept -> Rt {
    return normalize<Rt>(value);
  }

  export template <IsIntegral Rt, IsIntegral It>
  constexpr auto cast(It value) noexcept -> Rt {
    return scale<Rt>(value);
  }
} // namespace color