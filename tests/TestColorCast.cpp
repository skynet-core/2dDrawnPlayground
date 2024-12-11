#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cstdint>
#include <doctest/doctest.h>

import Literals;
import ColorUtilities;

using namespace color_literals;

TEST_CASE("Color Normalization") {

  SUBCASE("uint8_t to float") {
    constexpr auto color = 255_u8;
    constexpr auto normalized = color::normalize<float>(color);
    auto denormalized = color::denormalize<uint8_t>(normalized);
    CHECK(normalized == 1.0);
    CHECK(denormalized == color);
  }

  SUBCASE("uint8_t to double") {
    constexpr auto color = 255_u8;
    constexpr auto normalized = color::normalize<double>(color);
    auto denormalized = color::denormalize<uint8_t>(normalized);
    CHECK(normalized == 1.0);
    CHECK(denormalized == color);
  }

  SUBCASE("uint8_t to uint16_t") {
    constexpr auto color = 122_u8;
    constexpr auto scaled = color::scale<uint16_t>(color);
    auto casted = color::scale<uint8_t>(scaled);
    CHECK(scaled == 31232);
    CHECK(casted == color);
  }

  SUBCASE("uint16_t to uint8_t") {
    constexpr auto color = 25500_u16;
    auto scaled = color::scale<uint8_t>(color);
    auto casted = color::scale<uint16_t>(scaled);
    CHECK(scaled == 99);
    CHECK(casted == 25344); // color lost is possible ...
  }
}
