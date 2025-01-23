
#include <test_symbol_model.hpp>
#include <testing.hpp>

#include <ktl/audio/audio_channel.hpp>
#include <ktl/encoder.hpp>

TEST(Encoder_test, encodeAvailableSymbols) {
  // Throw a few symbols in the stream
  kazoo::SymbolStream<TestToken> s_stream{TEST_SYMBOL_MODEL};
  s_stream.addSymbol(TestToken::SYMBOL_00);
  s_stream.addSymbol(TestToken::SYMBOL_01);
  s_stream.addSymbol(TestToken::SYMBOL_01);
  s_stream.addSymbol(TestToken::SYMBOL_01);
  s_stream.addSymbol(TestToken::SYMBOL_11);

  EXPECT_EQ(s_stream.getNumSymbols(), 5);

  // Encode the symbols into the audio buffer
  kazoo::Encoder<TestToken> transcoder{TEST_SYMBOL_MODEL};
  kazoo::AudioChannel audio_channel;
  EXPECT_EQ(transcoder.encodeAvailableSymbols(s_stream, audio_channel), 5);

  // Ensure the symbols were popped from the stream
  EXPECT_EQ(s_stream.getNumSymbols(), 0);
}
