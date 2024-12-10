#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <concepts>

namespace contracts {
  struct RGB;
  struct RGBA;

  template <typename> struct is_tag_ {
    static constexpr auto value = false;
  };
  template <> struct is_tag_<RGB> {
    static constexpr auto value = true;
  };
  template <> struct is_tag_<RGBA> {
    static constexpr auto value = true;
  };
  template <typename Tp> constexpr auto is_tag_v = is_tag_<Tp>::value;

  template <typename Tp>
  concept IsTag = is_tag_v<Tp>;

  template <typename Tp>
  concept IsArithmetic = std::is_arithmetic_v<Tp>;
  template <typename Tp>
  concept IsIntegral = std::is_integral_v<Tp>;
  template <typename Tp>
  concept IsFloatingPoint = std::is_floating_point_v<Tp>;
} // namespace contracts

#endif
