#include "include/Pixel.hpp"
#include <cstdint>
#include <iostream>
#include <string>

using namespace std::string_literals;
using namespace pixel;

auto main() -> int {
  RGB<float> rgb{0.1F, 0.2F, 0.4F};
  std::cout << rgb << "\n";
  RGB<unsigned char> rgb8{};
  rgb8.from(rgb);
  std::cout << rgb8 << "\n";
  return 0;
}
