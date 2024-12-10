#ifndef PALETTE_HPP
#define PALETTE_HPP

namespace palette {
  namespace tags {
    struct RGB;
    struct RGBA;

    namespace traits {
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
    } // namespace traits
  } // namespace tags

  inline namespace concepts {
    template <typename Tp>
    concept IsTag = tags::traits::is_tag_v<Tp>;
  } // namespace concepts

  template <IsTag Tg> struct Palette;

  template <> struct Palette<tags::RGB> {
    static constexpr auto R = 0;        // NOLINT
    static constexpr auto G = 1;        // NOLINT
    static constexpr auto B = 2;        // NOLINT
    static constexpr auto CHANNELS = 3; // NOLINT
  };

  template <> struct Palette<tags::RGBA> {
    static constexpr auto R = 0;        // NOLINT
    static constexpr auto G = 1;        // NOLINT
    static constexpr auto B = 2;        // NOLINT
    static constexpr auto A = 3;        // NOLINT
    static constexpr auto CHANNELS = 4; // NOLINT
  };

  using RGB = Palette<tags::RGB>;
  using RGBA = Palette<tags::RGBA>;

} // namespace palette

#endif