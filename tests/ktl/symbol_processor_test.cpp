#include <test_symbol_table.hpp>
#include <testing.hpp>

#include <ktl/binary_stream.hpp>
#include <ktl/symbol_stream.hpp>

TEST(SymbolProcessor_test, addSymbols) {
  kazoo::SymbolStream<TestToken> processor(TEST_SYMBOL_TABLE);

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
  kazoo::SymbolStream<TestToken> processor(TEST_SYMBOL_TABLE);
  processor.addSymbol(TestToken::SYMBOL_11);
  processor.addSymbol(TestToken::SYMBOL_01);

  {
    kazoo::BinaryStream b_stream;

    processor.populateBinaryStream(b_stream, 1, false);
    EXPECT_EQ(b_stream.getNumBits(), 4);

    auto deq = b_stream.getStreamDataConst();
    EXPECT_EQ(deq.size(), 1);
    EXPECT_EQ(deq.at(0), 0b0111);
  }

  processor.addSymbol(TestToken::SYMBOL_10);
  processor.addSymbol(TestToken::SYMBOL_10);
  processor.addSymbol(TestToken::SYMBOL_10);

  {
    kazoo::BinaryStream b_stream;

    processor.populateBinaryStream(b_stream, 2, false);
    EXPECT_EQ(b_stream.getNumBits(), 10);

    auto deq = b_stream.getStreamDataConst();
    EXPECT_EQ(deq.size(), 2);
    EXPECT_EQ(deq.at(0), 0b10100111);
    EXPECT_EQ(deq.at(1), 0b10);
  }
}