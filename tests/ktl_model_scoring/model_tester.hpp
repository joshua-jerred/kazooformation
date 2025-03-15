/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file score.hpp
/// @date 2025-03-14

#pragma once

#include <common/logger.hpp>
#include <ktl/encoder.hpp>
#include <ktl/i_symbol_stream.hpp>
#include <ktl/models/symbol_model.hpp>

namespace kazoo {

class ModelTester : public kazoo::Logger {
 public:
  ModelTester(kazoo::ISymbolModel &symbol_model, kazoo::ISymbolStream &symbol_stream,
              kazoo::IEncoder &encoder)
      : Logger("ModelTester"),
        symbol_model_(symbol_model),
        symbol_stream_(symbol_stream),
        encoder_(encoder) {
  }

  void runAllTests();

  double getScore() {
    return score_;
  }

  void resetState() {
    symbol_stream_.clear();
    encoder_.clear();
  }

 private:
  static double getRandomDouble(double min, double max);
  std::string getRandomTestString();

  bool expectTrue(bool condition, std::string_view on_fail_message,
                  double penalty_on_fail = 1.0, double score_increase_on_pass = 0.1);

  bool helper_encodeStringToAudioChannel(const std::string &test_string,
                                         kazoo::AudioChannel &audio_channel);

  bool helper_decodeAudioChannelToString(const kazoo::AudioChannel &audio_channel,
                                         std::string &decoded_string);

  bool test_basicEncodeDecode();

  bool test_gaussianNoiseTolerance();

  const std::array<std::string_view, 5> test_strings_{"t", "Hello, World!", "test1",
                                                      "Longer Testing String", "abd"};

  double score_ = 0.0;

  kazoo::ISymbolModel &symbol_model_;
  kazoo::ISymbolStream &symbol_stream_;
  kazoo::IEncoder &encoder_;
};

};  // namespace kazoo