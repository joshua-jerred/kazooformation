
#include <iostream>

#include <testing.hpp>

#include <ktl/binary_stream.hpp>

TEST(BinaryStream_test, addData) {
  kazoo::BinaryStream stream;

  EXPECT_EQ(stream.getNumBits(), 0);
  EXPECT_EQ(stream.getNumBytes(), 0);
  EXPECT_TRUE(stream.isByteAligned());

  stream.addBits(0b1, 1);
  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 1);
  EXPECT_EQ(stream.getInputBitBufferSpace(), 7);
  EXPECT_FALSE(stream.isByteAligned());

  stream.addBits(0b1010, 4);
  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 5);
  EXPECT_EQ(stream.getInputBitBufferSpace(), 3);
  EXPECT_FALSE(stream.isByteAligned());
  // std::cout << stream << std::endl;

  stream.addBits(0b11, 2);
  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 7);
  EXPECT_EQ(stream.getInputBitBufferSpace(), 1);
  EXPECT_FALSE(stream.isByteAligned());
  // std::cout << stream << std::endl;

  stream.addBits(0b1, 1);  // We should now be byte aligned
  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 8);
  EXPECT_EQ(stream.getInputBitBufferSpace(), 0);
  EXPECT_TRUE(stream.isByteAligned());

  stream.addBits(0b1, 1);
  EXPECT_EQ(stream.getNumBytes(), 2);
  EXPECT_EQ(stream.getNumBits(), 9);
  EXPECT_EQ(stream.getInputBitBufferSpace(), 7);
  EXPECT_FALSE(stream.isByteAligned());

  uint8_t output = stream.getBytes(1).at(0);
  EXPECT_EQ(output, 0b11110101) << "actual output: " << std::bitset<8>(output);

  std::array<uint8_t, 2> input_bytes = {0xAA, 0xBB};

  EXPECT_THROW(stream.addBytes(input_bytes), std::runtime_error);
  stream.addBits(0b1111111, 7);  // Realign
  stream.addBytes(input_bytes);

  EXPECT_EQ(stream.getNumBytes(), 4);

  // std::cout << stream << std::endl;
}

/// @test Test the linear flow of the binary stream. If 3 bits are sent in, then
/// those same 3 bits should be able to be popped out. To avoid edge cases, only
/// front or back alignment may be off at any given time.
TEST(BinaryStream_test, linearFlow) {
  kazoo::BinaryStream stream;
  constexpr size_t bit_width = 3;
  stream.addBits(0b011, bit_width);
  ASSERT_EQ(stream.getNumBits(), 3);
  stream.addBits(0b111, bit_width);
  ASSERT_EQ(stream.getNumBits(), 6);
  stream.addBits(0b101, bit_width);
  ASSERT_EQ(stream.getNumBits(), 9);
  stream.addBits(0b101, bit_width);
  stream.addBits(0b101, bit_width);
  stream.addBits(0b111, bit_width);
  std::cout << "stream: " << stream << std::endl;
  ASSERT_EQ(stream.getNumBits(), 18);
  stream.pad();
  ASSERT_EQ(stream.getNumBits(), 24);
  EXPECT_TRUE(stream.isByteAligned());

  uint8_t output{0};

  // Pop the first 3 bits
  ASSERT_EQ(stream.getNumBits(), 24);
  {
    auto bit_opt = stream.popBit();
    EXPECT_TRUE(bit_opt.has_value());
    EXPECT_TRUE(bit_opt.value());  // ls bit first
    EXPECT_EQ(stream.getNumBits(), 23);
    EXPECT_FALSE(stream.isByteAligned());

    // pop the next few so we realign with bit_width
    EXPECT_TRUE(stream.popBits(output, bit_width - 1U));
    EXPECT_EQ(output, 0b01);
  }
  EXPECT_EQ(stream.getNumBits(), 21);

  EXPECT_TRUE(stream.popBits(output, bit_width));
  EXPECT_EQ(output, 0b111) << "The next 3 bits should be 111";

  EXPECT_EQ(stream.getNumBits(), 18);
  EXPECT_TRUE(stream.popBits(output, bit_width));
  EXPECT_EQ(output, 0b101) << "The next 3 bits should be 101";

  EXPECT_EQ(stream.getNumBits(), 15);
  EXPECT_TRUE(stream.popBits(output, bit_width));
  EXPECT_EQ(output, 0b101) << "The next 3 bits should be 101";

  EXPECT_EQ(stream.getNumBits(), 12);
  EXPECT_TRUE(stream.popBits(output, bit_width));
  EXPECT_EQ(output, 0b101) << "The next 3 bits should be 101";

  EXPECT_EQ(stream.getNumBits(), 9);
  EXPECT_TRUE(stream.popBits(output, bit_width));
  EXPECT_EQ(output, 0b111) << "The next 3 bits should be 111";

  // These 6 extra bits are from the padding.
  EXPECT_EQ(stream.getNumBits(), 6);
}