
#include <test_symbol_table.hpp>
#include <testing.hpp>

#include <ktl/transcoder.hpp>

TEST(Transcoder_test, encodeAvailableSymbols) {
  kazoo::SymbolStream<TestToken> s_stream{TEST_SYMBOL_TABLE};
  kazoo::Transcoder<TestToken> transcoder{s_stream};

  s_stream.addSymbol(TestToken::SYMBOL_00);
  s_stream.addSymbol(TestToken::SYMBOL_01);
}
