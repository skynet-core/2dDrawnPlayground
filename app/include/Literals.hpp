#ifndef LITERALS_HPP
#define LITERALS_HPP

#include <cstdint>

namespace color {
  inline namespace color_literals {
    constexpr auto operator"" _u8(const unsigned long long value) -> uint8_t {
      return static_cast<uint8_t>(value);
    }
    constexpr auto operator"" _u16(const unsigned long long value) -> uint16_t {
      return static_cast<uint16_t>(value);
    }

    constexpr auto operator"" _u32(const unsigned long long value) -> uint32_t {
      return static_cast<uint32_t>(value);
    }

    constexpr auto operator"" _u64(const unsigned long long value) -> uint64_t {
      return value;
    }

    constexpr auto operator"" _f32(const long double value) -> float {
      return static_cast<float>(value);
    }

    constexpr auto operator"" _f64(const long double value) -> double {
      return static_cast<double>(value);
    }

  } // namespace color_literals
} // namespace color
#endif