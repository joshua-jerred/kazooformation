#include <testing.hpp>

#include <symbol_processor.hpp>

TEST(SymbolProcessor_test, addSymbols) {
  kazoo::SymbolStream processor;

  EXPECT_EQ(processor.getNumBits(), 0);
  EXPECT_EQ(processor.getNumBytes(), 0);
  EXPECT_EQ(processor.getNumSymbols(), 0);
  EXPECT_EQ(processor.getNumUpspecifiedBits(), 0);
  EXPECT_TRUE(processor.isByteAligned());

  processor.addSymbol(kazoo::Token::SYMBOL_11);

  EXPECT_EQ(processor.getNumBits(), 2);
  EXPECT_EQ(processor.getNumBytes(), 1);
  EXPECT_EQ(processor.getNumSymbols(), 1);
  EXPECT_EQ(processor.getNumUpspecifiedBits(), 6);
  EXPECT_FALSE(processor.isByteAligned());

  // pad out to 8 bits
  processor.addSymbol(kazoo::Token::SYMBOL_00);
  processor.addSymbol(kazoo::Token::SYMBOL_00);
  processor.addSymbol(kazoo::Token::SYMBOL_10);

  EXPECT_EQ(processor.getNumBits(), 8);
  EXPECT_EQ(processor.getNumBytes(), 1);
  EXPECT_EQ(processor.getNumSymbols(), 4);
  EXPECT_EQ(processor.getNumUpspecifiedBits(), 0);
  EXPECT_TRUE(processor.isByteAligned());

  // Ensure that we can't get more bytes than we have
  std::vector<uint8_t> bytes;
  EXPECT_FALSE(processor.getBytes(bytes, 2));

  // Get the first byte
  EXPECT_TRUE(processor.getBytes(bytes, 1));
  EXPECT_EQ(bytes.size(), 1);
  EXPECT_EQ(bytes.at(0), 0b11000010);

  // Add additional symbols
  processor.addSymbol(kazoo::Token::SYMBOL_10);
  processor.addSymbol(kazoo::Token::SYMBOL_10);
  EXPECT_EQ(processor.getNumBits(), 12);
  EXPECT_EQ(processor.getNumBytes(), 2);
  EXPECT_EQ(processor.getNumSymbols(), 6);
  EXPECT_EQ(processor.getNumUpspecifiedBits(), 4);
  EXPECT_FALSE(processor.isByteAligned());

  EXPECT_TRUE(processor.getBytes(bytes, 2));
  EXPECT_EQ(bytes.size(), 2);
  EXPECT_EQ(bytes.at(0), 0b11000010);
  EXPECT_EQ(bytes.at(1), 0b10100000);

  // std::cout << processor << std::endl;

  // re-byte align
  // std::cout << processor << std::endl;
  // processor.addSymbol(kazoo::Token::SYMBOL_00);
  // processor.addSymbol(kazoo::Token::SYMBOL_00);
  // EXPECT_FALSE(processor.isByteAligned());
  // processor.addSymbol(kazoo::Token::SYMBOL_11);
  // EXPECT_TRUE(processor.isByteAligned());
  // std::cout << processor << std::endl;
}