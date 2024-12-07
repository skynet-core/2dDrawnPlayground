#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <array>
#include <cstddef>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace pixel {
  namespace color {
    template <typename Tp>
    concept Arithmetic = std::is_arithmetic_v<Tp>;
    template <typename Tp>
    concept Integral = std::is_integral_v<Tp>;
    template <typename Tp>
    concept FloatingPoint = std::is_floating_point_v<Tp>;

    template <FloatingPoint Tp, Integral Up>
    inline constexpr auto normalize(Up &&value) noexcept -> Tp {
      return static_cast<Tp>(value) / std::numeric_limits<Up>::max();
    }

    template <Integral Tp, FloatingPoint Up>
    inline constexpr auto denormalize(Up value) noexcept -> Tp {
      return static_cast<Tp>(value * std::numeric_limits<Tp>::max());
    }

    template <Integral Rt, Integral It>
    inline constexpr auto scale(It value) noexcept -> Rt {
      return normalize<long double>(value) * std::numeric_limits<Rt>::max();
    }

    template <Integral Rt, FloatingPoint It>
    inline constexpr auto cast(It value) noexcept -> Rt {
      return denormalize<Rt>(value);
    }

    template <FloatingPoint Rt, Integral It>
    inline constexpr auto cast(It value) noexcept -> Rt {
      return normalize<Rt>(value);
    }

    template <Integral Rt, Integral It>
      requires(!std::is_same_v<Rt, It>)
    inline constexpr auto cast(It value) noexcept -> Rt {
      return scale<Rt>(value);
    }

    template <Integral Rt, Integral It>
      requires std::is_same_v<Rt, It>
    inline constexpr auto cast(It value) noexcept -> Rt {
      return value;
    }

  } // namespace color

  namespace colorspace {
    struct RGB;
    struct RGBA;

    constexpr auto RGB_N_CHANNELS = 3;
    constexpr auto RGBA_N_CHANNELS = 4;

    template <typename Tp> struct is_tag_ {
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
    concept Tag = is_tag_v<Tp>;

    template <Tag Tp> constexpr auto num_channels() noexcept -> std::size_t {
      if constexpr (std::is_same_v<Tp, RGB>) {
        return RGB_N_CHANNELS;
      } else if constexpr (std::is_same_v<Tp, RGBA>) {
        return RGBA_N_CHANNELS;
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

    template <color::Arithmetic Tp, colorspace::Tag Tg>
    struct PixelBase : std::array<Tp, colorspace::num_channels<Tg>()> {
      PixelBase() noexcept = default;

      template <typename... Tps>
        requires(std::is_same_v<Tps, Tp> && ...) &&
                (sizeof...(Tps) == colorspace::num_channels<Tg>())
      constexpr PixelBase(Tps &&...args) noexcept
          : std::array<Tp, colorspace::num_channels<Tg>()>{args...} {}

      constexpr static auto num_channels() noexcept -> std::size_t {
        return colorspace::num_channels<Tg>();
      }

      template <color::Arithmetic Up> constexpr void fill(Up &&value) noexcept {
        std::fill(std::begin(*this), std::end(*this),
                  color::cast<Tp>(std::forward<Up>(value)));
      }

      template <color::Arithmetic Up>
      constexpr void from(const PixelBase<Up, Tg> &rhs) noexcept {
        for (std::size_t i = 0; i < num_channels(); ++i) {
          (*this)[i] = color::cast<Tp, Up>(rhs[i]);
        }
      }
    };

    template <color::Arithmetic Tp, colorspace::Tag T> struct Pixel;

    template <color::Arithmetic Tp>
    struct Pixel<Tp, colorspace::RGB> : PixelBase<Tp, colorspace::RGB> {
      using PixelBase<Tp, colorspace::RGB>::PixelBase;

      constexpr auto r() -> Tp & { return (*this)[colorspace::rgba::R]; }

      constexpr auto g() -> Tp & { return (*this)[colorspace::rgba::G]; }

      constexpr auto b() -> Tp & { return (*this)[colorspace::rgba::B]; }

      constexpr auto r() const -> const Tp & {
        return const_cast<const Tp>((*this).r());
      }

      constexpr auto g() const -> const Tp & {
        return const_cast<const Tp>((*this).g());
      }

      constexpr auto b() const -> const Tp & {
        return const_cast<const Tp>((*this).b());
      }

      constexpr static auto num_channels() noexcept -> std::size_t {
        return colorspace::num_channels<colorspace::RGB>();
      }
    };

    template <color::Arithmetic Tp>
    struct Pixel<Tp, colorspace::RGBA> : PixelBase<Tp, colorspace::RGBA> {
      using PixelBase<Tp, colorspace::RGBA>::PixelBase;

      constexpr auto r() -> Tp & { return (*this)[colorspace::rgba::R]; }

      constexpr auto g() -> Tp & { return (*this)[colorspace::rgba::G]; }

      constexpr auto b() -> Tp & { return (*this)[colorspace::rgba::B]; }

      constexpr auto a() -> Tp & { return (*this)[colorspace::rgba::A]; }

      constexpr auto r() const -> const Tp & {
        return const_cast<const Tp>((*this).r());
      }

      constexpr auto g() const -> const Tp & {
        return const_cast<const Tp>((*this).g());
      }

      constexpr auto b() const -> const Tp & {
        return const_cast<const Tp>((*this).b());
      }

      constexpr auto a() const -> const Tp & {
        return const_cast<const Tp>((*this).a());
      }

      constexpr static auto num_channels() noexcept -> std::size_t {
        return colorspace::num_channels<colorspace::RGBA>();
      }
    };

    template <std::size_t... Is>
    inline constexpr auto
    ids_to_tuple([[maybe_unused]] std::index_sequence<Is...> _args) noexcept {
      return std::make_tuple(Is...);
    }

    template <color::Arithmetic Tp, colorspace::Tag T>
    auto operator<<(std::ostream &out,
                    const Pixel<Tp, T> &pix) -> std::ostream & {
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

  template <color::Arithmetic Tp>
  using RGB = detail::Pixel<Tp, colorspace::RGB>;

  template <color::Arithmetic Tp>
  using RGBA = detail::Pixel<Tp, colorspace::RGBA>;

} // namespace pixel

#endif
