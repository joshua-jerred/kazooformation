
#include <test_symbol_table.hpp>
#include <testing.hpp>

#include <ktl/transcoder.hpp>

TEST(Transcoder_test, encodeAvailableSymbols) {
  // Throw a few symbols in the stream
  kazoo::SymbolStream<TestToken> s_stream{TEST_SYMBOL_TABLE};
  s_stream.addSymbol(TestToken::SYMBOL_00);
  s_stream.addSymbol(TestToken::SYMBOL_01);
  s_stream.addSymbol(TestToken::SYMBOL_01);
  s_stream.addSymbol(TestToken::SYMBOL_01);
  s_stream.addSymbol(TestToken::SYMBOL_11);

  EXPECT_EQ(s_stream.getNumSymbols(), 5);

  // Encode the symbols into the audio buffer
  kazoo::Transcoder<TestToken> transcoder{TEST_SYMBOL_TABLE};
  EXPECT_EQ(transcoder.encodeAvailableSymbols(s_stream), 5);

  // Ensure the symbols were popped from the stream
  EXPECT_EQ(s_stream.getNumSymbols(), 0);
}
