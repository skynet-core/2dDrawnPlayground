#ifndef PIXEL_HPP
#define PIXEL_HPP

#include "Contracts.hpp"
#include "Normalization.hpp"
#include "Palette.hpp"
#include <algorithm>
#include <array>
#include <ostream>
#include <iomanip>

namespace pixel {
  using namespace contracts;
  using namespace palette;
  using namespace palette::concepts;

  namespace detail {

    template <IsArithmetic Tp, IsTag Tg>
    struct PixelBase : std::array<Tp, Palette<Tg>::CHANNELS> {
      using ColorPalette = Palette<Tg>;

      PixelBase() noexcept = default;

      template <typename... Tps>
        requires((std::is_convertible_v<Tps, Tp> && ...) &&
                 (sizeof...(Tps) == ColorPalette::CHANNELS))
      constexpr explicit PixelBase(Tps... args) noexcept
          : std::array<Tp, ColorPalette::CHANNELS>{static_cast<Tp>(args)...} {}

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
        return ColorPalette::CHANNELS;
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

    template <IsArithmetic Tp>
    struct Pixel<Tp, tags::RGB> : PixelBase<Tp, tags::RGB> {
      using PixelBase<Tp, tags::RGB>::PixelBase;
      using ColorPalette = typename PixelBase<Tp, tags::RGB>::ColorPalette;

      constexpr auto r() const -> const Tp & { return (*this)[ColorPalette::R]; }

      constexpr auto g() const -> const Tp & { return (*this)[ColorPalette::G]; }

      constexpr auto b() const -> const Tp & { return (*this)[ColorPalette::B]; }

      constexpr auto r() -> Tp & {
        return const_cast<PixelBase<Tp, RGB> *>(*this)->r();
      }

      constexpr auto g() -> Tp & {
        return const_cast<PixelBase<Tp, RGB> *>(*this)->g();
      }

      constexpr auto b() -> Tp & {
        return const_cast<PixelBase<Tp, RGB> *>(*this)->b();
      }
    };

    template <IsArithmetic Tp>
    struct Pixel<Tp, tags::RGBA> : PixelBase<Tp, tags::RGBA> {
      using PixelBase<Tp, tags::RGBA>::PixelBase;
      using ColorPalette = typename PixelBase<Tp, tags::RGBA>::ColorPalette;

      constexpr auto r() const -> const Tp & {
        return (*this)[ColorPalette::R];
      }

      constexpr auto g() const -> const Tp & {
        return (*this)[ColorPalette::G];
      }

      constexpr auto b() const -> const Tp & {
        return (*this)[ColorPalette::B];
      }

      constexpr auto a() const -> const Tp & {
        return (*this)[ColorPalette::A];
      }

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
    };

    template <std::size_t... Is>
    constexpr auto
    ids_to_tuple([[maybe_unused]] std::index_sequence<Is...> _args) noexcept {
      return std::make_tuple(Is...);
    }

    template <IsArithmetic Tp, IsTag T>
    auto operator<<(std::ostream &out, const Pixel<Tp, T> &pix)
        -> std::ostream & {
      constexpr auto num_channels = Pixel<Tp, T>::num_channels();
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

  template <IsArithmetic Tp> using RGBPixel = detail::Pixel<Tp, tags::RGB>;

  template <IsArithmetic Tp> using RGBAPixel = detail::Pixel<Tp, tags::RGBA>;

} // namespace pixel

#endif
