#include "include/PPM.hpp"
#include <cstdint>
#include <functional>
#include <iostream>

using namespace std::string_literals;
using namespace pixel;

auto main() -> int {
  constexpr auto width = 1024;
  constexpr auto height = 1024;
  ppm::Image<uint16_t> image{};
  image.resize(width, height);
  {
    image.drawFunc([](int xPos, int yPos, ppm::Pixel<uint16_t> &pix) {
      auto value = (xPos ^ yPos) << 8; // scale color a bit
      pix = ppm::Pixel<uint16_t>{value, value, value};
    });
    if (!image.save("bit_and.ppm")) {
      std::cerr << "Failed to save image\n";
    }
    image.drawFunc([](int xPos, int yPos, ppm::Pixel<uint16_t> &pix) {
      auto value = (xPos | yPos) << 8; // scale color a bit
      pix = ppm::Pixel<uint16_t>{value, value, value};
    });
    if (!image.save("bit_or.ppm")) {
      std::cerr << "Failed to save image\n";
    }
    image.drawFunc([](int xPos, int yPos, ppm::Pixel<uint16_t> &pix) {
      auto value = (xPos | yPos) << 8; // scale color a bit
      pix = ppm::Pixel<uint16_t>{value, value, value};
    });
    if (!image.save("bit_xor.ppm")) {
      std::cerr << "Failed to save image\n";
    }
  }
  return 0;
}
