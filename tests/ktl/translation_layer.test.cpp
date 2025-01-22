/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#include <array>

#include <testing.hpp>

#include <ktl/translation_layer.hpp>

TEST(TranslationLayer_test, encodeToWav) {
  kazoo::TranslationLayer tl{kazoo::TranslationLayer::ModelType::TESTING};

  std::array<uint8_t, 4> data = {0, 1, 2, 3};

  EXPECT_EQ(tl.getStats().num_bytes, 0);
  EXPECT_EQ(tl.getStats().symbols_last_encoded, 0);

  tl.addData(data);
  EXPECT_EQ(tl.getStats().num_bytes, 4);
  EXPECT_EQ(tl.getStats().symbols_last_encoded, 0);

  tl.encode();
  EXPECT_EQ(tl.getStats().num_bytes, 0);
  EXPECT_EQ(tl.getStats().symbols_last_encoded, data.size() * 8);
}