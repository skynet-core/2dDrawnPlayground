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

    void fill(const Pixel<Tp> &color) {
      std::fill(pixels_.begin(), pixels_.end(), color);
    }

    void fill(Tp value) {
      std::fill(pixels_.begin()->begin(), pixels_.end()->end(), value);
    }

    void drawFunc(const std::function<int(int, int)> &func) {
      for (auto xPos = 0; xPos < width_; ++xPos) {
        for (auto yPos = 0; yPos < height_; ++yPos) {
          const auto val = pixel::color::cast<Tp, double>(
              pixel::color::normalize<double>(static_cast<Tp>(
                  func(xPos, yPos) % std::numeric_limits<Tp>::max())));
          this->pixels_[xPos + (yPos * width_)] = Pixel<Tp>{val, val, val};
        }
      }
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

    std::vector<Pixel<Tp>> pixels_{};
    int width_{}, height_{};
  };
} // namespace ppm

#endif