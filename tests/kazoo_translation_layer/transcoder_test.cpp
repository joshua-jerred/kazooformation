
#include <test_symbol_table.hpp>
#include <testing.hpp>

#include <kazoo_translation_layer/transcoder.hpp>

TEST(Transcoder_test, encodeAvailableSymbols) {
  kazoo::SymbolStream<TestToken> s_stream{TEST_SYMBOL_TABLE};
  kazoo::Transcoder<TestToken> transcoder{s_stream};
}
