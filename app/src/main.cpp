#include <CLI/CLI.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

import Generators;
import Image.Ppm;

using namespace std::string_literals;
using namespace pixel;

constexpr auto DEFAULT_WIDTH = 512U;
constexpr auto DEFAULT_HEIGHT = 512U;

namespace {
  using DrawFuncT =
      std::function<void(std::size_t, std::size_t, ppm::Pixel<uint8_t> &)>;
  using RegistryT = std::unordered_map<std::string, DrawFuncT>;

  auto makeDrawFuncWithBinaryOp(
      const std::function<uint8_t(uint8_t, uint8_t)> &operation) -> DrawFuncT {
    return [operation](std::size_t xPos, std::size_t yPos,
                       ppm::Pixel<uint8_t> &pix) {
      auto value =
          operation(static_cast<uint8_t>(xPos), static_cast<uint8_t>(yPos));
      pix = ppm::Pixel<uint8_t>{value, value, value};
    };
  }

  auto getFunctionsRegistry() -> const RegistryT & {
    static RegistryT const functions{
        {"bit_xor"s, makeDrawFuncWithBinaryOp(std::bit_xor<>{})},
        {"bit_and"s, makeDrawFuncWithBinaryOp(std::bit_and<>{})},
        {"bit_or"s, makeDrawFuncWithBinaryOp(std::bit_or<>{})},
    };
    return functions;
  }
} // namespace

auto main(int argc, char **argv) -> int {
  const auto &functions = getFunctionsRegistry();

  std::size_t width{};
  std::size_t height{};
  std::string outFile{};
  std::string inFile{};
  std::string funcName{};

  CLI::App app{"2D Drawing playground"s};
  app.add_option("-w,--width"s, width, "Image width"s)
      ->default_val(DEFAULT_WIDTH);
  app.add_option("-H,--height"s, height, "Image height"s)
      ->default_val(DEFAULT_HEIGHT);
  app.add_option("-o,--output"s, outFile, "Output file name"s)
      ->default_val("output.ppm"s);
  app.add_option("-i,--input"s, inFile, "Input file name"s);
  app.add_option("-f,--function"s, funcName, "Function name"s)
      ->default_val("bit_xor"s);

  CLI11_PARSE(app, argc, argv);
  ppm::Image<uint8_t> image{};
  if (!inFile.empty()) {
    if (image.load(inFile)) {
      std::cerr << "Failed to load image\n";
      return -1;
    }
  } else {
    image.resize(width, height);
  }

  const auto func = functions.find(funcName);
  if (func != functions.end()) {
    std::cout << "Using function: " << funcName << '\n';
    image.drawFunc(func->second);
    if (!image.save(outFile)) {
      std::cerr << "Failed to save image\n";
    }
  }

  if (!image.save(outFile)) {
    std::cerr << "Failed to save image\n";
    return -2;
  }

  return 0;
}
