#ifndef PIXEL_HPP
#define PIXEL_HPP

#include "Contracts.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <ranges>
#include <stdfloat>
#include <tuple>
#include <type_traits>
#include <utility>

namespace pixel {
  using namespace contracts;
  namespace color {

    inline namespace color_literals {
      constexpr auto operator"" _u8(const unsigned long long value) -> uint8_t {
        return static_cast<uint8_t>(value);
      }
      constexpr auto operator"" _u16(const unsigned long long value)
          -> uint16_t {
        return static_cast<uint16_t>(value);
      }

      constexpr auto operator"" _u32(const unsigned long long value)
          -> uint32_t {
        return static_cast<uint32_t>(value);
      }

      constexpr auto operator"" _u64(const unsigned long long value)
          -> uint64_t {
        return value;
      }

      constexpr auto operator"" _f16(const long double value)
          -> std::float16_t {
        return static_cast<std::float16_t>(value);
      }

      constexpr auto operator"" _f32(const long double value)
          -> std::float32_t {
        return static_cast<std::float32_t>(value);
      }

      constexpr auto operator"" _f64(const long double value)
          -> std::float64_t {
        return static_cast<std::float64_t>(value);
      }

      constexpr auto operator"" _f128(const long double value)
          -> std::float128_t {
        return value;
      }

    } // namespace color_literals

    template <IsFloatingPoint Tp, IsIntegral Up>
    constexpr auto normalize(Up value) noexcept -> Tp {
      return static_cast<Tp>(value) / std::numeric_limits<Up>::max();
    }

    template <IsIntegral Tp, IsFloatingPoint Up>
    constexpr auto denormalize(Up value) noexcept -> Tp {
      return static_cast<Tp>(value * std::numeric_limits<Tp>::max());
    }

    template <IsIntegral Rt, IsIntegral It>
    constexpr auto scale(It value) noexcept -> Rt {
      return static_cast<Rt>(normalize<long double>(value) *
                             std::numeric_limits<Rt>::max());
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
      requires(!std::is_same_v<Rt, It>)
    constexpr auto cast(It value) noexcept -> Rt {
      return scale<Rt>(value);
    }

    template <IsIntegral Rt, IsIntegral It>
      requires std::is_same_v<Rt, It>
    constexpr auto cast(It value) noexcept -> Rt {
      return value;
    }

  } // namespace color

  namespace colorspace {
    constexpr auto RGB_N_CHANNELS = 3;
    constexpr auto RGBA_N_CHANNELS = 4;

    template <IsTag Tg> constexpr auto num_channels() noexcept -> std::size_t {
      if constexpr (std::is_same_v<Tg, RGB>) {
        return RGB_N_CHANNELS;
      } else if constexpr (std::is_same_v<Tg, RGBA>) {
        return RGBA_N_CHANNELS;
      } else {
        static_assert(is_tag_v<Tg>);
      }
    }

    namespace rgba {
      constexpr auto R = 0; // NOLINT
      constexpr auto G = 1; // NOLINT
      constexpr auto B = 2; // NOLINT
      constexpr auto A = 3; // NOLINT
    } // namespace rgba
  } // namespace colorspace

  namespace detail {

    using namespace colorspace;

    template <IsArithmetic Tp, IsTag Tg>
    struct PixelBase : std::array<Tp, colorspace::num_channels<Tg>()> {
      PixelBase() noexcept = default;

      template <typename... Tps>
        requires((std::is_convertible_v<Tps, Tp> && ...) &&
                 (sizeof...(Tps) == colorspace::num_channels<Tg>()))
      constexpr explicit PixelBase(Tps... args) noexcept
          : std::array<Tp, colorspace::num_channels<Tg>()>{
                static_cast<Tp>(args)...} {}

      constexpr PixelBase(const PixelBase &) noexcept = default;
      constexpr PixelBase(PixelBase &&) noexcept = default;
      constexpr auto operator=(const PixelBase &) noexcept
          -> PixelBase & = default;
      constexpr auto operator=(PixelBase &&) noexcept -> PixelBase & = default;
      ~PixelBase() noexcept = default;

      template <typename Ut>
      // NOLINTNEXTLINE
      constexpr PixelBase(const PixelBase<Ut, Tg> &other) noexcept {
        this->from(other);
      }

      template <typename Ut>
      constexpr auto operator=(const PixelBase<Ut, Tg> &other) noexcept
          -> PixelBase & {
        this->from(other);
        return *this;
      }

      constexpr static auto num_channels() noexcept -> std::size_t {
        return colorspace::num_channels<Tg>();
      }

      template <IsArithmetic Up> constexpr void fill(Up &&value) noexcept {
        std::fill(std::begin(*this), std::end(*this),
                  color::cast<Tp>(std::forward<Up>(value)));
      }

      template <IsArithmetic Up>
        requires(!std::same_as<Up, Tp>)
      constexpr void from(const PixelBase<Up, Tg> &rhs) noexcept {
        std::ranges::transform(
            std::begin(rhs), std::end(rhs), std::begin(*this),
            [](auto val) { return color::cast<Tp, Up>(val); });
      }

      template <IsArithmetic Up>
        requires std::same_as<Up, Tp>
      constexpr void from(const PixelBase<Up, Tg> &rhs) noexcept {
        std::ranges::copy(rhs, *this);
      }
    };

    template <IsArithmetic Tt, IsTag Tg>
    void swap(PixelBase<Tt, Tg> &lhs, PixelBase<Tt, Tg> &rhs) noexcept {
      lhs.swap(rhs);
    }

    template <IsArithmetic Tp, IsTag T> struct Pixel;

    template <IsArithmetic Tp> struct Pixel<Tp, RGB> : PixelBase<Tp, RGB> {
      using PixelBase<Tp, RGB>::PixelBase;

      constexpr auto r() const -> const Tp & { return (*this)[rgba::R]; }

      constexpr auto g() const -> const Tp & { return (*this)[rgba::G]; }

      constexpr auto b() const -> const Tp & { return (*this)[rgba::B]; }

      constexpr auto r() -> Tp & {
        return const_cast<PixelBase<Tp, RGB> *>(*this)->r();
      }

      constexpr auto g() -> Tp & {
        return const_cast<PixelBase<Tp, RGB> *>(*this)->g();
      }

      constexpr auto b() -> Tp & {
        return const_cast<PixelBase<Tp, RGB> *>(*this)->b();
      }

      constexpr static auto num_channels() noexcept -> std::size_t {
        return colorspace::num_channels<RGB>();
      }
    };

    template <IsArithmetic Tp> struct Pixel<Tp, RGBA> : PixelBase<Tp, RGBA> {
      using PixelBase<Tp, RGBA>::PixelBase;

      constexpr auto r() const -> const Tp & { return (*this)[rgba::R]; }

      constexpr auto g() const -> const Tp & { return (*this)[rgba::G]; }

      constexpr auto b() const -> const Tp & { return (*this)[rgba::B]; }

      constexpr auto a() const -> const Tp & { return (*this)[rgba::A]; }

      constexpr auto r() -> Tp & {
        return const_cast<PixelBase<Tp, RGBA> *>(*this)->r();
      }

      constexpr auto g() -> Tp & {
        return const_cast<PixelBase<Tp, RGBA> *>(*this)->g();
      }

      constexpr auto b() -> Tp & {
        return const_cast<PixelBase<Tp, RGBA> *>(*this)->b();
      }

      constexpr auto a() -> Tp & {
        return const_cast<PixelBase<Tp, RGBA> *>(*this)->a();
      }

      constexpr static auto num_channels() noexcept -> std::size_t {
        return colorspace::num_channels<RGBA>();
      }
    };

    template <std::size_t... Is>
    constexpr auto
    ids_to_tuple([[maybe_unused]] std::index_sequence<Is...> _args) noexcept {
      return std::make_tuple(Is...);
    }

    template <IsArithmetic Tp, IsTag T>
    auto operator<<(std::ostream &out, const Pixel<Tp, T> &pix)
        -> std::ostream & {
      constexpr auto num_channels = colorspace::num_channels<T>();
      if constexpr (std::is_floating_point_v<Tp>) {
        std::apply(
            [&out, &pix](auto... ids) {
              out << std::fixed
                  << std::setprecision(std::numeric_limits<Tp>::digits10);
              (..., (out << pix[ids] << ", "));
            },
            ids_to_tuple(std::make_index_sequence<num_channels - 1>{}));
        out << pix[num_channels - 1];
        return out;
      } else {
        std::apply(
            [&out, &pix](auto... ids) {
              (..., (out << static_cast<int>(pix[ids]) << ", "));
            },
            ids_to_tuple(std::make_index_sequence<num_channels - 1>{}));
        out << static_cast<int>(pix[num_channels - 1]);
        return out;
      }
    } // namespace detail

  } // namespace detail

  template <IsArithmetic Tp> using RGBPixel = detail::Pixel<Tp, RGB>;

  template <IsArithmetic Tp> using RGBAPixel = detail::Pixel<Tp, RGBA>;

} // namespace pixel

#endif
