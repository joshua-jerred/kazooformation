/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#include <array>

#include <testing.hpp>

#include <ktl/translation_layer.hpp>

TEST(TranslationLayer_test, encodeToWav) {
  const std::string TEST_FILE_PATH = "translation_layer_encode.test.wav";
  if (std::filesystem::exists(TEST_FILE_PATH)) {
    std::filesystem::remove(TEST_FILE_PATH);
  }
  ASSERT_FALSE(std::filesystem::exists(TEST_FILE_PATH));

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

  tl.saveWav(TEST_FILE_PATH);
  EXPECT_TRUE(std::filesystem::exists(TEST_FILE_PATH));
}

TEST(TranslationLayer_test, encodeAndDecodeWav) {
  const std::string TEST_FILE_PATH =
      "translation_layer_encode_and_decode.test.wav";
  if (std::filesystem::exists(TEST_FILE_PATH)) {
    std::filesystem::remove(TEST_FILE_PATH);
  }
  ASSERT_FALSE(std::filesystem::exists(TEST_FILE_PATH));

  constexpr std::array<uint8_t, 4> INPUT_DATA = {0, 1, 2, 3};

  // First, encode the data to a wav file
  {
    kazoo::TranslationLayer tl{kazoo::TranslationLayer::ModelType::TESTING};
    ASSERT_EQ(tl.getStats().num_bytes, 0);
    ASSERT_EQ(tl.getStats().symbols_last_encoded, 0);
    tl.addData(INPUT_DATA);
    ASSERT_EQ(tl.getStats().num_bytes, 4);
    ASSERT_EQ(tl.getStats().symbols_last_encoded, 0);
    tl.encode();
    ASSERT_EQ(tl.getStats().num_bytes, 0);
    ASSERT_EQ(tl.getStats().symbols_last_encoded, INPUT_DATA.size() * 8);
    tl.saveWav(TEST_FILE_PATH);
    ASSERT_TRUE(std::filesystem::exists(TEST_FILE_PATH));
  }

  // Now, decode the data from the wav file
  kazoo::TranslationLayer tl2{kazoo::TranslationLayer::ModelType::TESTING};
  ASSERT_EQ(tl2.getStats().num_bytes, 0);

  tl2.loadAndDecodeWav(TEST_FILE_PATH);
}