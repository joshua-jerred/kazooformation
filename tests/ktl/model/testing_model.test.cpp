#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/wav_file.hpp>
#include <ktl/encoder.hpp>
#include <ktl/models/testing_model.hpp>

TEST(TestingModel_test, encode_and_decode) {
  const auto model = kazoo::model::Testing::Model{};
  static constexpr size_t SYM_COUNT = 18;
  const std::string TEST_WAV_FILE = "TestingModel_test.encodeAndDecode.wav";

  // Delete the test file if it exists
  if (std::filesystem::exists(TEST_WAV_FILE)) {
    std::filesystem::remove(TEST_WAV_FILE);
  }
  ASSERT_FALSE(std::filesystem::exists(TEST_WAV_FILE));

  using Token = kazoo::model::Testing::Token;
  const std::array<Token, SYM_COUNT> symbols = {
      Token::SYMBOL_0, Token::SYMBOL_0, Token::SYMBOL_1, Token::SYMBOL_0,
      Token::SYMBOL_1, Token::SYMBOL_1, Token::SYMBOL_0, Token::SYMBOL_0,
      Token::SYMBOL_0, Token::SYMBOL_0, Token::SYMBOL_1, Token::SYMBOL_1,
      Token::SYMBOL_0, Token::SYMBOL_1, Token::SYMBOL_1, Token::SYMBOL_0,
      Token::SYMBOL_1, Token::SYMBOL_1};

  // First, encode the file
  {
    // Throw a few symbols in the stream
    kazoo::SymbolStream<kazoo::model::Testing::Token> s_stream{model};
    for (const auto& symbol : symbols) {
      s_stream.addSymbol(symbol);
    }

    EXPECT_EQ(s_stream.getNumSymbols(), SYM_COUNT);

    // Encode the symbols into the audio buffer
    kazoo::Encoder<kazoo::model::Testing::Token> encoder{model};
    kazoo::AudioChannel audio_channel;
    EXPECT_EQ(encoder.encodeAvailableSymbols(s_stream, audio_channel),
              SYM_COUNT);
    EXPECT_EQ(s_stream.getNumSymbols(),
              0);  // Ensure the symbols were popped from the stream
    EXPECT_EQ(audio_channel.getNumSamples(),
              kazoo::model::Testing::SAMPLES_PER_SYMBOL * SYM_COUNT);

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
              kazoo::model::Testing::SAMPLES_PER_SYMBOL * SYM_COUNT);

    // Decode the audio file using the testing model
    kazoo::SymbolStream<kazoo::model::Testing::Token> s_stream{model};
    kazoo::model::Testing::Model m{};
    m.decodeAudioToSymbols(wav_file, s_stream);
    ASSERT_EQ(s_stream.getNumSymbols(), SYM_COUNT);

    // Make sure the symbols are correct
    kazoo::model::Testing::Token token;
    for (size_t i = 0; i < SYM_COUNT; i++) {
      ASSERT_TRUE(s_stream.popSymbol(token));
      EXPECT_EQ(token, symbols.at(i));
    }
  }
}
