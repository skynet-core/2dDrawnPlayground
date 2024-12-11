module;
export module Palette;

namespace palette {
  namespace tags {
    export struct RGB;
    export struct RGBA;

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
    export template <typename Tp>
    concept IsTag = tags::traits::is_tag_v<Tp>;
  } // namespace concepts

  export template <IsTag Tg> struct Palette;

  export template <> struct Palette<tags::RGB> {
    static constexpr auto R = 0;        // NOLINT
    static constexpr auto G = 1;        // NOLINT
    static constexpr auto B = 2;        // NOLINT
    static constexpr auto CHANNELS = 3; // NOLINT
  };

  export template <> struct Palette<tags::RGBA> {
    static constexpr auto R = 0;        // NOLINT
    static constexpr auto G = 1;        // NOLINT
    static constexpr auto B = 2;        // NOLINT
    static constexpr auto A = 3;        // NOLINT
    static constexpr auto CHANNELS = 4; // NOLINT
  };

  export using RGB = Palette<tags::RGB>;
  export using RGBA = Palette<tags::RGBA>;

} // namespace palette