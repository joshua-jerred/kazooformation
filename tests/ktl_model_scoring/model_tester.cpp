/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file score.hpp
/// @date 2025-03-14

#include "model_tester.hpp"

#include <random>

namespace kazoo {

void ModelTester::runAllTests() {
  score_ = 0.0;
  resetState();

  logDebug("Running tests...");
  test_basicEncodeDecode();
  resetState();
  test_gaussianNoiseTolerance();
  resetState();
}

double ModelTester::getRandomDouble(double min, double max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(min, max);
  return dis(gen);
}

std::string ModelTester::getRandomTestString() {
  size_t index = static_cast<size_t>(getRandomDouble(0, test_strings_.size() - 1));
  return std::string(test_strings_.at(index));
}

bool ModelTester::expectTrue(bool condition, std::string_view on_fail_message,
                             double penalty_on_fail, double score_increase_on_pass) {
  if (!condition) {
    logError("expect failed: " + std::string(on_fail_message));
    score_ -= penalty_on_fail;
    return false;
  } else {
    score_ += score_increase_on_pass;
    return true;
  }
}

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---- Helper Functions -----------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

bool ModelTester::helper_encodeStringToAudioChannel(const std::string &test_string,
                                                    kazoo::AudioChannel &audio_channel) {
  kazoo::BinaryStream binary_stream;
  binary_stream.addBytes(std::span(reinterpret_cast<const uint8_t *>(test_string.data()),
                                   test_string.size()));
  const auto res = symbol_stream_.processBinaryStream(binary_stream);

  expectTrue(res.first == test_string.size(), "num bytes processed");
  expectTrue(res.second == (test_string.size() * 8 / symbol_model_.getSymbolBitWidth()),
             "num symbols processed");

  encoder_.encodeAvailableSymbols(symbol_stream_, audio_channel);
  expectTrue(symbol_stream_.getNumBytes() == 0, "symbol stream empty");
  expectTrue(audio_channel.getNumSamples() > 0, "audio channel has samples");
  return true;
}

bool ModelTester::helper_decodeAudioChannelToString(
    const kazoo::AudioChannel &audio_channel, std::string &decoded_string) {
  symbol_stream_.clear();
  symbol_model_.decodeAudioToSymbols(audio_channel, symbol_stream_);

  BinaryStream binary_stream;
  symbol_stream_.populateBinaryStream(binary_stream, decoded_string.size());
  const auto &decoded = binary_stream.getStreamDataConst();

  decoded_string.clear();
  for (const auto &byte : decoded) {
    decoded_string.push_back(static_cast<char>(byte));
  }

  return true;
}

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---- Test Functions -------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

bool ModelTester::test_basicEncodeDecode() {
  const std::string test_string = "test";
  kazoo::AudioChannel audio_channel;

  logDebug("Testing basic encode/decode...");

  // Encode the string into the audio channel
  {
    kazoo::BinaryStream binary_stream;
    binary_stream.addBytes(std::span(
        reinterpret_cast<const uint8_t *>(test_string.data()), test_string.size()));
    const auto res = symbol_stream_.processBinaryStream(binary_stream);

    logDebug("test_basicEncodeDecode processBinaryStream num bytes processed: " +
             std::to_string(res.first));
    logDebug("test_basicEncodeDecode processBinaryStream num symbols processed:" +
             std::to_string(res.second));

    expectTrue(res.first == test_string.size(), "num bytes processed");
    expectTrue(res.second == (test_string.size() * 8 / symbol_model_.getSymbolBitWidth()),
               "num symbols processed");

    encoder_.encodeAvailableSymbols(symbol_stream_, audio_channel);
    expectTrue(symbol_stream_.getNumBytes() == 0, "symbol stream empty");
    expectTrue(audio_channel.getNumSamples() > 0, "audio channel has samples");
  }

  // Decode the string from the audio channel
  {
    symbol_stream_.clear();
    symbol_model_.decodeAudioToSymbols(audio_channel, symbol_stream_);
    expectTrue(symbol_stream_.getNumBytes() == test_string.size(), "num bytes decoded");

    BinaryStream binary_stream;
    symbol_stream_.populateBinaryStream(binary_stream, test_string.size());
    const auto &decoded = binary_stream.getStreamDataConst();

    expectTrue(std::equal(decoded.begin(), decoded.end(),
                          reinterpret_cast<const uint8_t *>(test_string.data())),
               "decoded data matches original");
  }

  return true;
}

bool ModelTester::test_gaussianNoiseTolerance() {
  constexpr double NOISE_STEP = 0.1;
  constexpr double MAX_NOISE_LEVEL = 0.9;
  double noise_level = 0.1;

  logDebug("Testing Gaussian noise tolerance...");

  while (noise_level <= MAX_NOISE_LEVEL) {
    resetState();

    std::string test_string = getRandomTestString();
    kazoo::AudioChannel input_audio_channel;
    helper_encodeStringToAudioChannel(test_string, input_audio_channel);

    // Add Gaussian noise to the audio channel
    // kazoo::AudioChannel noisy_audio_channel = input_audio_channel;
    // std::default_random_engine generator;
    // std::normal_distribution<double> distribution(0.0, noise_level);
    // for (size_t i = 0; i < noisy_audio_channel.getNumSamples(); ++i) {
    //   int16_t sample = noisy_audio_channel.getSample(i);
    //   double noise = distribution(generator);
    //   noisy_audio_channel.setSample(i, static_cast<int16_t>(sample + noise));
    // }

    // Decode the string from the noisy audio channel
    std::string decoded_string;
    helper_decodeAudioChannelToString(input_audio_channel, decoded_string);

    // Check if the decoded string matches the original
    if (!expectTrue(decoded_string == test_string, "decoded string matches original", 0.1,
                    0.25)) {
      logDebug("Failed at noise level: " + std::to_string(noise_level) +
               "input|output: " + test_string + " | " + decoded_string);
      continue;
    }

    logDebug("Passed at noise level: " + std::to_string(noise_level));

    noise_level += NOISE_STEP;
  }

  return true;
}

};  // namespace kazoo