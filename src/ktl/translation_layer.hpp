/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <iostream>
#include <span>

namespace kazoo {

class TranslationLayer {
 public:
  enum class KazooModel {
    UNKNOWN = 0,
    BINARY
  };

  TranslationLayer(const KazooModel model)
      : kazoo_model_(model) {}

  void addData(const std::span<const uint8_t> &data) {
    (void)data;
    std::cout << "Adding data to translation layer" << std::endl;
  }

 private:
  const KazooModel kazoo_model_;
};

}  // namespace kazoo