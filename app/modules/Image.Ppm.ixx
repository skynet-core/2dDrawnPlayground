module;

#include <climits>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <system_error>
#include <vector>

export module Image.Ppm;

import Contracts;
import Pixel;
import Operators;

namespace ppm {
  using namespace std::filesystem;
  using namespace std::string_literals;

  template <typename T>
  concept IsPPMIntegral =
      contracts::IsIntegral<T> &&
      std::numeric_limits<T>::max() <= std::numeric_limits<uint16_t>::max();

  export template <IsPPMIntegral T> using Pixel = pixel::RGBPixel<T>;

  export template <IsPPMIntegral Tp>
    requires(sizeof(Tp) <= 2)
  struct Image {

    using Container = std::vector<Pixel<Tp>>;
    constexpr static auto MAGIC{"P3"};

    Image() = default;
    Image(std::size_t width, std::size_t height)
        : pixels_(width * height), width_{width}, height_{height} {}

    auto operator[](std::size_t xPos, std::size_t yPos) const
        -> const Pixel<Tp> & {
      return pixels_[xPos + (yPos * width_)];
    }

    auto operator[](std::size_t xPos, std::size_t yPos) -> Pixel<Tp> & {
      return const_cast<Image &>(*this)->operator[](xPos, yPos);
    }

    void resize(std::size_t width, std::size_t height) {
      width_ = width;
      height_ = height;
      pixels_.resize(width * height);
    }

    [[nodiscard]] auto width() const -> std::size_t { return width_; }

    [[nodiscard]] auto height() const -> std::size_t { return height_; }

    void fill(const Pixel<Tp> &color) {
      std::fill(pixels_.begin(), pixels_.end(), color);
    }

    void fill(Tp value) {
      std::fill(pixels_.begin()->begin(), pixels_.end()->end(), value);
    }

    void drawFunc(const std::function<void(std::size_t xPos, std::size_t yPos,
                                           Pixel<Tp> &pix)> &func) {
      for (auto xPos = 0; xPos < width_; ++xPos) {
        for (auto yPos = 0; yPos < height_; ++yPos) {
          func(xPos, yPos, this->pixels_[xPos + (yPos * width_)]);
        }
      }
    }

    [[nodiscard]] auto begin() const -> typename Container::const_iterator {
      return pixels_.cbegin();
    }

    [[nodiscard]] auto end() const -> typename Container::const_iterator {
      return pixels_.cend();
    }

    [[nodiscard]] auto crbegin() const ->
        typename Container::const_reverse_iterator {
      return pixels_.rbegin();
    }

    [[nodiscard]] auto crend() const ->
        typename Container::const_reverse_iterator {
      return pixels_.crend();
    }

    [[nodiscard]] auto begin() -> typename Container::iterator {
      return pixels_.begin();
    }

    [[nodiscard]] auto end() -> typename Container::iterator {
      return pixels_.end();
    }

    [[nodiscard]] auto rbegin() -> typename Container::reverse_iterator {
      return pixels_.rbegin();
    }

    [[nodiscard]] auto rend() -> typename Container::reverse_iterator {
      return pixels_.rend();
    }

    auto save(const path &fileName) -> bool {
      ensureParentPath(fileName);
      if (std::ofstream file{fileName, std::ios::out | std::ios::trunc}; file) {
        file << MAGIC << "\n"
             << width_ << ' ' << height_ << '\n'
             << static_cast<int>(std::numeric_limits<Tp>::max()) << '\n';

        for (const auto &pixel : pixels_) {
          file << static_cast<int>(pixel.r()) << ' '
               << static_cast<int>(pixel.g()) << ' '
               << static_cast<int>(pixel.b()) << ' ';
        }
        file.seekp(-1, std::ios_base::end);
        return true;
      }
      return false;
    }

    auto load(const path &fileName) -> std::error_code {
      if (std::ifstream file{fileName, std::ios::in}; file) {
        std::string magic;
        std::size_t width{};
        std::size_t height{};
        std::size_t max{};
        file >> magic >> width >> height >> max;
        if (magic != std::string{MAGIC}) {
          return std::error_code{EINVAL, std::generic_category()};
        }
        resize(width, height);
        std::function<Tp(int, std::size_t)> operation = bits::NoOp{};
        auto shift{0};
        if (max > std::numeric_limits<Tp>::max()) {
          shift = calculateBitShiftRight(max, std::numeric_limits<Tp>::max());
          operation = bits::ShiftRight{};
        }
        if (max < std::numeric_limits<Tp>::max()) {
          shift = calculateBitShiftLeft(max, std::numeric_limits<Tp>::max());
          operation = bits::ShiftLeft{};
        }

        int redColor{};
        int greenColor{};
        int blueColor{};
        for (auto &pixel : pixels_) {
          file >> redColor >> greenColor >> blueColor;
          pixel = Pixel<Tp>{operation(redColor, shift),
                            operation(greenColor, shift),
                            operation(blueColor, shift)};
        }
        return {};
      }
      return std::error_code{ENOENT, std::generic_category()};
    }

  private:
    constexpr static auto calculateBitShiftRight(long long max, long long min) {
      std::size_t shift{0};
      while (max >> shift > min) {
        shift += CHAR_BIT;
      }
      return shift;
    }

    constexpr static auto calculateBitShiftLeft(long long min, long long max) {
      std::size_t shift{0};
      while (min << shift > max) {
        shift += CHAR_BIT;
      }
      return shift;
    }

    static void ensureParentPath(const path &path) {
      if (path.has_parent_path()) {
        std::error_code code;
        create_directories(path.parent_path(), code);
      }
    }

    Container pixels_{};
    std::size_t width_{};
    std::size_t height_{};
  };
} // namespace ppm