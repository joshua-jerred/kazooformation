
#include <iostream>

#include <testing.hpp>

#include <ktl/deframer.hpp>
#include <ktl/ktl_frame.hpp>

TEST(Deframer_test, helloWorldDeframe) {
  kazoo::KtlFrame input_frame{"Hello World!"};
  std::vector<uint8_t> input_data = input_frame.encodeFrame(2, 2);

  // for (const auto& byte : input_data) {
  // std::cout << std::dec << static_cast<int>(byte);
  // std::cout << " (" << std::bitset<8>(byte) << ", ";
  // std::cout << std::hex << static_cast<int>(byte) << ")" << std::endl;
  // }

  kazoo::BinaryStream input_stream{};

  // Add a byte for confusion
  // (binary stream must be byte aligned before adding bytes)
  input_stream.addBits(0b00100001, 8);

  // add the actual hello world
  input_stream.addBytes(input_data);

  kazoo::Deframer deframer{};
  EXPECT_TRUE(deframer.processInput(input_stream));
}

// TEST(Deframer_test, helloWorldDeframe_misaligned) {
//   kazoo::KtlFrame input_frame{"Hello World!"};
//   std::vector<uint8_t> input_data = input_frame.encodeFrame(2, 2);
//   kazoo::BinaryStream input_stream{};

//   // Add a byte for confusion as the binary stream must be byte aligned before
//   // adding bytes.
//   input_stream.addBits(0b00100001, 8);
//   input_stream.addBytes(input_data);

//   kazoo::Deframer deframer{};
//   EXPECT_TRUE(deframer.processInput(input_stream));
// }