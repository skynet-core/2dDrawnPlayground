#include "include/PPM.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using namespace std::string_literals;
using namespace pixel;
using namespace pixel::color::color_literals;

auto main() -> int {
  constexpr auto width = 1024;
  constexpr auto height = 1024;
  ppm::Image<uint8_t> image{};
  image.resize(width, height);
  {
    image.drawFunc(std::bit_and<>{});
    if (!image.save("bit_and.ppm")) {
      std::cerr << "Failed to save image\n";
    }
    image.drawFunc(std::bit_or<>{});
    if (!image.save("bit_or.ppm")) {
      std::cerr << "Failed to save image\n";
    }
    image.drawFunc(std::bit_xor<>{});
    if (!image.save("bit_xor.ppm")) {
      std::cerr << "Failed to save image\n";
    }
  }
  return 0;
}
