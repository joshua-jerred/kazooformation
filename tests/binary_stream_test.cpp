
#include <iostream>

#include <testing.hpp>

#include <binary_stream.hpp>

TEST(BinaryStream_test, addData) {
  kazoo::BinaryStream stream;

  EXPECT_EQ(stream.getNumBits(), 0);
  EXPECT_EQ(stream.getNumBytes(), 0);
  EXPECT_TRUE(stream.isByteAligned());

  stream.addBits(0b1, 1);

  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 1);
  EXPECT_EQ(stream.getNumUpspecifiedBits(), 7);
  EXPECT_FALSE(stream.isByteAligned());

  stream.addBits(0b1010, 4);

  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 5);
  EXPECT_EQ(stream.getNumUpspecifiedBits(), 3);
  EXPECT_FALSE(stream.isByteAligned());
  // std::cout << stream << std::endl;

  stream.addBits(0b11, 2);

  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 7);
  EXPECT_EQ(stream.getNumUpspecifiedBits(), 1);
  EXPECT_FALSE(stream.isByteAligned());
  // std::cout << stream << std::endl;

  stream.addBits(0b1, 1);

  EXPECT_EQ(stream.getNumBytes(), 1);
  EXPECT_EQ(stream.getNumBits(), 8);
  EXPECT_EQ(stream.getNumUpspecifiedBits(), 0);
  EXPECT_TRUE(stream.isByteAligned());

  // We should now be byte aligned

  stream.addBits(0b1, 1);

  EXPECT_EQ(stream.getNumBytes(), 2);
  EXPECT_EQ(stream.getNumBits(), 9);
  EXPECT_EQ(stream.getNumUpspecifiedBits(), 7);
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
