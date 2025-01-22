/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#include <array>

#include <testing.hpp>

#include <ktl/translation_layer.hpp>

TEST(TranslationLayer_test, encodeToWav) {
  kazoo::TranslationLayer tl{kazoo::TranslationLayer::KazooModel::BINARY};

  std::array<uint8_t, 4> data = {0, 1, 2, 3};
  // tl.addData(data);
}