/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#include <testing.hpp>

#include <ktl/model/binary_model.hpp>
#include <ktl/symbol_stream.hpp>

TEST(BinaryModel_test, SymbolStreamIntegration) {
  using Token = kazoo::model::Binary::Token;

  kazoo::model::Binary::Model BINARY_MODEL;
  kazoo::SymbolStream<kazoo::model::Binary::Token> processor(BINARY_MODEL);

  // Test the initial state
  EXPECT_EQ(processor.getNumBits(), 0);
  EXPECT_EQ(processor.getNumBytes(), 0);
  EXPECT_EQ(processor.getNumSymbols(), 0);

  processor.addSymbol(Token::SYMBOL_1);
  EXPECT_EQ(processor.getNumBits(), 1);
  EXPECT_EQ(processor.getNumBytes(), 1);
  EXPECT_EQ(processor.getNumSymbols(), 1);

  processor.addSymbol(Token::SYMBOL_0);
  processor.addSymbol(Token::SYMBOL_0);
  processor.addSymbol(Token::SYMBOL_1);
  EXPECT_EQ(processor.getNumBits(), 4);
  EXPECT_EQ(processor.getNumBytes(), 1);
  EXPECT_EQ(processor.getNumSymbols(), 4);

  processor.addSymbol(Token::SYMBOL_1);
  processor.addSymbol(Token::SYMBOL_1);
  processor.addSymbol(Token::SYMBOL_1);
  processor.addSymbol(Token::SYMBOL_1);
  EXPECT_EQ(processor.getNumBits(), 8);
  EXPECT_EQ(processor.getNumBytes(), 1);
  EXPECT_EQ(processor.getNumSymbols(), 8);

  processor.addSymbol(Token::SYMBOL_1);
  EXPECT_EQ(processor.getNumBits(), 9);
  EXPECT_EQ(processor.getNumBytes(), 2);
  EXPECT_EQ(processor.getNumSymbols(), 9);
}