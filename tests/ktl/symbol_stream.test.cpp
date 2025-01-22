#include <test_symbol_model.hpp>
#include <testing.hpp>

#include <ktl/binary_stream.hpp>
#include <ktl/symbol_stream.hpp>

TEST(SymbolStream_test, addSymbols) {
  kazoo::SymbolStream<TestToken> stream(TEST_SYMBOL_MODEL);

  EXPECT_EQ(stream.getNumBits(), 0);
  EXPECT_EQ(stream.getNumBytes(), 0);
  EXPECT_EQ(stream.getNumSymbols(), 0);

  stream.addSymbol(TestToken::SYMBOL_11);
  EXPECT_EQ(stream.getNumBits(), 2);
  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumSymbols(), 1);

  // pad out to 8 bits
  stream.addSymbol(TestToken::SYMBOL_00);
  stream.addSymbol(TestToken::SYMBOL_00);
  stream.addSymbol(TestToken::SYMBOL_10);
  EXPECT_EQ(stream.getNumBits(), 8);
  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumSymbols(), 4);
}

TEST(SymbolStream_test, populateBinaryStream) {
  kazoo::SymbolStream<TestToken> processor(TEST_SYMBOL_MODEL);
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

/// @todo
TEST(SymbolStream_test, processBinaryStream) {
  GTEST_SKIP() << "Not implemented";

  kazoo::BinaryStream expected_stream;
  {
    kazoo::SymbolStream<TestToken> processor(TEST_SYMBOL_MODEL);
    processor.addSymbol(TestToken::SYMBOL_11);
    processor.addSymbol(TestToken::SYMBOL_01);
    processor.addSymbol(TestToken::SYMBOL_10);
    processor.addSymbol(TestToken::SYMBOL_10);
    processor.addSymbol(TestToken::SYMBOL_10);
    processor.populateBinaryStream(expected_stream, 2, false);
    ASSERT_EQ(expected_stream.getNumBits(), 10);
    const auto in_bytes = expected_stream.getStreamDataConst();
    ASSERT_EQ(in_bytes.size(), 2);
    ASSERT_EQ(in_bytes.at(0), 0b10100111);
    ASSERT_EQ(in_bytes.at(1), 0b10);
  }

  kazoo::SymbolStream<TestToken> processor(TEST_SYMBOL_MODEL);
  auto res_pare = processor.processBinaryStream(expected_stream);
  EXPECT_EQ(res_pare.first, 2) << "Bytes processed should be 2";
  EXPECT_EQ(res_pare.second, 5) << "Symbols processed should be 5";
  // kazoo::BinaryStream processor_stream;
}