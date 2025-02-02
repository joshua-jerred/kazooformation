/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file k1_model.test.cpp
/// @date 2025-02-02

#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/wav_file.hpp>
#include <ktl/encoder.hpp>
#include <ktl/models/k1_model.hpp>

TEST(K1Model_test, encode_and_decode) {
  const auto model = kazoo::model::K1Model::Model{};
  const std::string TEST_WAV_FILE = "K1Model_test.encode_and_decode.wav";
  static constexpr size_t SYM_COUNT = 18;

  // Delete the test file if it exists
  if (std::filesystem::exists(TEST_WAV_FILE)) {
    std::filesystem::remove(TEST_WAV_FILE);
  }
  ASSERT_FALSE(std::filesystem::exists(TEST_WAV_FILE));

  using Token = kazoo::model::K1Model::Token;
  const std::array<Token, SYM_COUNT> symbols = {
      Token::SYMBOL_0, Token::SYMBOL_0, Token::SYMBOL_1, Token::SYMBOL_0,
      Token::SYMBOL_1, Token::SYMBOL_1, Token::SYMBOL_0, Token::SYMBOL_0,
      Token::SYMBOL_0, Token::SYMBOL_0, Token::SYMBOL_1, Token::SYMBOL_1,
      Token::SYMBOL_0, Token::SYMBOL_1, Token::SYMBOL_1, Token::SYMBOL_0,
      Token::SYMBOL_1, Token::SYMBOL_1};

  // First, encode the file
  {
    // Throw a few symbols in the stream
    kazoo::SymbolStream<Token> s_stream{model};
    for (const auto& symbol : symbols) {
      s_stream.addSymbol(symbol);
    }

    EXPECT_EQ(s_stream.getNumSymbols(), SYM_COUNT);

    // Encode the symbols into the audio buffer
    kazoo::Encoder<kazoo::model::K1Model::Token> encoder{model};
    kazoo::AudioChannel audio_channel;
    EXPECT_EQ(encoder.encodeAvailableSymbols(s_stream, audio_channel),
              SYM_COUNT);
    EXPECT_EQ(s_stream.getNumSymbols(),
              0);  // Ensure the symbols were popped from the stream
    EXPECT_EQ(audio_channel.getNumSamples(),
              kazoo::model::K1Model::SAMPLES_PER_SYMBOL * SYM_COUNT);

    kazoo::WavFile wav_file;
    wav_file.loadFromAudioChannel(audio_channel);
    wav_file.write(TEST_WAV_FILE);
  }

  // Check for file existence
  ASSERT_TRUE(std::filesystem::exists(TEST_WAV_FILE));

  // Then attempt to decode the file
  {
    // Load the audio file
    kazoo::WavFile wav_file;
    wav_file.read(TEST_WAV_FILE);
    ASSERT_EQ(wav_file.getNumSamples(),
              kazoo::model::K1Model::SAMPLES_PER_SYMBOL * SYM_COUNT);

    // Decode the audio file using the testing model
    kazoo::SymbolStream<kazoo::model::K1Model::Token> s_stream{model};
    kazoo::model::K1Model::Model m{};
    m.decodeAudioToSymbols(wav_file, s_stream);
    ASSERT_EQ(s_stream.getNumSymbols(), SYM_COUNT);

    // Make sure the symbols are correct
    kazoo::model::K1Model::Token token;
    for (size_t i = 0; i < SYM_COUNT; i++) {
      ASSERT_TRUE(s_stream.popSymbol(token));
      EXPECT_EQ(token, symbols.at(i));
    }
  }
}