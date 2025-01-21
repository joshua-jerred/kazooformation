#include <testing.hpp>

#include <kazoo_translation_layer/binary_stream.hpp>
#include <kazoo_translation_layer/symbol_processor.hpp>

enum class TestToken : uint32_t {
  UNKNOWN = 0,
  SYMBOL_00,
  SYMBOL_01,
  SYMBOL_10,
  SYMBOL_11,
  _SYMBOL_COUNT
};

static const kazoo::SymbolTable<TestToken> symbol_table{
    {{TestToken::SYMBOL_00, 0},
     {TestToken::SYMBOL_01, 1},
     {TestToken::SYMBOL_10, 2},
     {TestToken::SYMBOL_11, 3}},
    2};

TEST(SymbolProcessor_test, addSymbols) {
  kazoo::SymbolStream<TestToken> processor(symbol_table);

  EXPECT_EQ(processor.getNumBits(), 0);
  EXPECT_EQ(processor.getNumBytes(), 0);
  EXPECT_EQ(processor.getNumSymbols(), 0);

  processor.addSymbol(TestToken::SYMBOL_11);

  EXPECT_EQ(processor.getNumBits(), 2);
  EXPECT_EQ(processor.getNumBytes(), 1);
  EXPECT_EQ(processor.getNumSymbols(), 1);

  // pad out to 8 bits
  processor.addSymbol(TestToken::SYMBOL_00);
  processor.addSymbol(TestToken::SYMBOL_00);
  processor.addSymbol(TestToken::SYMBOL_10);

  EXPECT_EQ(processor.getNumBits(), 8);
  EXPECT_EQ(processor.getNumBytes(), 1);
  EXPECT_EQ(processor.getNumSymbols(), 4);
}

TEST(SymbolProcessor_test, populateBinaryStream) {
  kazoo::SymbolStream<TestToken> processor(symbol_table);
  processor.addSymbol(TestToken::SYMBOL_11);
  {
    kazoo::BinaryStream b_stream;
    processor.populateBinaryStream(b_stream, 1, false);

    // EXPECT_EQ(b_stream.getNumBits(), 2);
  }
}