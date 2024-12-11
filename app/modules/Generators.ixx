module;

#include <cstddef>
#include <random>
#include <vector>

export module Generators;

namespace noise {
  export template <typename Engine = std::mt19937> struct Generator {
    struct Config {
      std::size_t width{};  // NOLINT
      std::size_t height{}; // NOLINT
      double min{};         // NOLINT
      double max{};         // NOLINT

      Config(std::size_t width_, std::size_t height_, double min_, double max_)
          : width{width_}, height{height_}, min{min_}, max{max_} {}
    };

    Generator(unsigned int seed, Config config)
        : engine_{seed > 0 ? seed : std::random_device{}()},
          dist_{config.min, config.max}, width_{config.width},
          height_{config.height} {}

    [[nodiscard]] auto width() const -> std::size_t { return width_; }
    [[nodiscard]] auto height() const -> std::size_t { return height_; }

    auto noise() -> std::vector<double> {
      std::vector<double> noise(width() * height());
      for (auto yPos = 0; yPos < height(); ++yPos) {
        for (auto xPos = 0; xPos < width(); ++xPos) {
          noise[xPos + (width() * yPos)] = dist(engine_);
        }
      }
      return noise;
    }

  private:
    Engine engine_;
    std::uniform_real_distribution<> dist_;
    std::size_t width_;
    std::size_t height_;
  };
} // namespace noise