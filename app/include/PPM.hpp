#ifndef PPM_HPP
#define PPM_HPP

#include "Pixel.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <vector>

namespace ppm {
  using namespace std::filesystem;

  template <typename T>
  concept IsPPMIntegral =
      contracts::IsIntegral<T> &&
      std::numeric_limits<T>::max() <= std::numeric_limits<uint16_t>::max();

  template <IsPPMIntegral T> using Pixel = pixel::RGBPixel<T>;

  template <IsPPMIntegral Tp>
    requires(sizeof(Tp) <= 2)
  struct Image {

    using Container = std::vector<Pixel<Tp>>;

    Image() = default;
    Image(const int width, const int height)
        : pixels_(width * height), width_{width}, height_{height} {}

    auto operator[](const int xPos, const int yPos) const -> const Pixel<Tp> & {
      return pixels_[xPos + (yPos * width_)];
    }

    auto operator[](const int xPos, const int yPos) -> Pixel<Tp> & {
      return const_cast<Image &>(*this)->operator[](xPos, yPos);
    }

    void resize(const int width, const int height) {
      width_ = width;
      height_ = height;
      pixels_.resize(width * height);
    }

    [[nodiscard]] auto width() const -> int { return width_; }

    [[nodiscard]] auto height() const -> int { return height_; }

    void fill(const Pixel<Tp> &color) {
      std::fill(pixels_.begin(), pixels_.end(), color);
    }

    void fill(Tp value) {
      std::fill(pixels_.begin()->begin(), pixels_.end()->end(), value);
    }

    void drawFunc(
        const std::function<void(int xPos, int yPos, Pixel<Tp> &pix)> &func) {
      for (auto xPos = 0; xPos < width_; ++xPos) {
        for (auto yPos = 0; yPos < height_; ++yPos) {
          func(xPos, yPos, this->pixels_[xPos + (yPos * width_)]);
        }
      }
    }

    [[nodiscard]] auto begin() const -> Container::const_iterator {
      return pixels_.cbegin();
    }

    [[nodiscard]] auto end() const -> Container::const_iterator {
      return pixels_.cend();
    }

    [[nodiscard]] auto crbegin() const -> Container::const_reverse_iterator {
      return pixels_.rbegin();
    }

    [[nodiscard]] auto crend() const -> Container::const_reverse_iterator {
      return pixels_.crend();
    }

    [[nodiscard]] auto begin() -> Container::iterator {
      return pixels_.begin();
    }

    [[nodiscard]] auto end() -> Container::iterator { return pixels_.end(); }

    [[nodiscard]] auto rbegin() -> Container::reverse_iterator {
      return pixels_.rbegin();
    }

    [[nodiscard]] auto rend() -> Container::reverse_iterator {
      return pixels_.rend();
    }

    auto save(const path &fileName) -> bool {
      ensureParentPath(fileName);
      if (std::ofstream file{fileName, std::ios::out | std::ios::trunc}; file) {
        file << "P3\n"
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

  private:
    static void ensureParentPath(const path &path) {
      if (path.has_parent_path()) {
        std::error_code code;
        create_directories(path.parent_path(), code);
      }
    }

    Container pixels_{};
    int width_{}, height_{};
  };
} // namespace ppm

#endif