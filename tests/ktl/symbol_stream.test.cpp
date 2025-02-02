#include <testing.hpp>

#include <ktl/binary_stream.hpp>
#include <ktl/models/symbol_model.hpp>
#include <ktl/symbol_stream.hpp>

enum class TestToken : uint32_t {
  UNKNOWN = 0,
  SYMBOL_00,
  SYMBOL_01,
  SYMBOL_10,
  SYMBOL_11,
  _SYMBOL_COUNT
};

static const kazoo::SymbolModel<TestToken> TEST_SYMBOL_MODEL{
    {{TestToken::SYMBOL_00, 0},
     {TestToken::SYMBOL_01, 1},
     {TestToken::SYMBOL_10, 2},
     {TestToken::SYMBOL_11, 3}},
    2};

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

TEST(SymbolStream_test, processBinaryStream) {
  kazoo::BinaryStream in_stream;
  in_stream.addBits(0b11, 2);
  in_stream.addBits(0b10, 2);
  in_stream.addBits(0b01, 2);
  in_stream.addBits(0b00, 2);

  in_stream.addBits(0b0000, 4);
  in_stream.addBits(0b1111, 4);

  ASSERT_EQ(in_stream.getNumBytes(), 2);

  kazoo::SymbolStream<TestToken> processor(TEST_SYMBOL_MODEL);
  auto res_pare = processor.processBinaryStream(in_stream);
  EXPECT_EQ(res_pare.first, 2) << "Bytes processed";
  EXPECT_EQ(res_pare.second, 8) << "Symbols processed";

  TestToken token;
  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_11);
  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_10);
  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_01);
  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_00);

  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_00);
  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_00);
  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_11);
  EXPECT_TRUE(processor.popSymbol(token));
  EXPECT_EQ(token, TestToken::SYMBOL_11);
}

/// @test Validate that an array can be converted to and from symbols.
TEST(SymbolStream_test, arrayValidation) {
  const std::array<uint8_t, 10U> INPUT_DATA = {0x00, 0x01, 0x02, 0x03, 0x04,
                                               0x05, 0x06, 0x07, 0x08, 0x09};
  kazoo::BinaryStream in_stream;
  in_stream.addBytes(INPUT_DATA);

  kazoo::SymbolStream<TestToken> input_processor(TEST_SYMBOL_MODEL);
  input_processor.processBinaryStream(in_stream);
  EXPECT_EQ(input_processor.getNumBytes(), 10);
  EXPECT_EQ(input_processor.getNumSymbols(), 40);

  kazoo::SymbolStream<TestToken> output_processor(TEST_SYMBOL_MODEL);
  TestToken token;
  while (input_processor.popSymbol(token)) {
    output_processor.addSymbol(token);
  }

  EXPECT_EQ(output_processor.getNumBytes(), 10);
  EXPECT_EQ(output_processor.getNumSymbols(), 40);
  EXPECT_EQ(output_processor.getNumBits(), 80);

  kazoo::BinaryStream out_stream;
  output_processor.populateBinaryStream(out_stream, 10, false);
  EXPECT_EQ(out_stream.getNumBytes(), 10);

  auto out_data = out_stream.getStreamDataConst();
  EXPECT_EQ(INPUT_DATA.size(), out_data.size());
  for (size_t i = 0; i < INPUT_DATA.size(); i++) {
    EXPECT_EQ(INPUT_DATA.at(i), out_data.at(i));
  }
}