/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file translation_layer.test.cpp
/// @date 2025-01-25

#include <array>

#include <testing.hpp>

#include <ktl/translation_layer.hpp>

TEST(TranslationLayer_test, encode_to_wav) {
  GTEST_SKIP();

  const std::string TEST_FILE_PATH = "TranslationLayer_test.encode_to_wav.wav";
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

TEST(TranslationLayer_test, encode_and_decode_wav) {
  GTEST_SKIP();

  const std::string TEST_FILE_PATH =
      "translation_layer_encode_and_decode.test.wav";
  if (std::filesystem::exists(TEST_FILE_PATH)) {
    std::filesystem::remove(TEST_FILE_PATH);
  }
  ASSERT_FALSE(std::filesystem::exists(TEST_FILE_PATH));

  constexpr std::array<uint8_t, 4> INPUT_DATA = {0, 0, 1, 1};

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

  // Load the wav file and decode it
  {
    std::vector<uint8_t> decoded_data{};
    tl2.loadAndDecodeWav(TEST_FILE_PATH, decoded_data);
    ASSERT_EQ(decoded_data.size(), INPUT_DATA.size());
    for (size_t i = 0; i < decoded_data.size(); ++i) {
      EXPECT_EQ(decoded_data.at(i), INPUT_DATA.at(i));
    }
  }
}

/// @test The very first test with actual kazoo symbols
TEST(TranslationLayer_test, kazoo_hello_world) {
  GTEST_SKIP();

  const std::string TEST_FILE_PATH =
      "TranslationLayer_test.kazoo_hello_world.wav";
  if (std::filesystem::exists(TEST_FILE_PATH)) {
    std::filesystem::remove(TEST_FILE_PATH);
  }
  ASSERT_FALSE(std::filesystem::exists(TEST_FILE_PATH));

  kazoo::TranslationLayer tl{kazoo::TranslationLayer::ModelType::K1_MODEL};
  const std::array<uint8_t, 12> INPUT_DATA = {'H', 'e', 'l', 'l', 'o', ' ',
                                              'W', 'o', 'r', 'l', 'd', '!'};

  tl.addData(INPUT_DATA);
  tl.encode();
  tl.saveWav(TEST_FILE_PATH);
  EXPECT_TRUE(std::filesystem::exists(TEST_FILE_PATH));

  kazoo::TranslationLayer tl2{kazoo::TranslationLayer::ModelType::K1_MODEL};
  std::vector<uint8_t> decoded_data{};
  tl2.loadAndDecodeWav(TEST_FILE_PATH, decoded_data);
  std::string decoded_str{};
  for (const auto& byte : decoded_data) {
    decoded_str.push_back(byte);
  }

  EXPECT_EQ(decoded_str, "Hello World!");
}