#include "gtest/gtest.h"

#include "symbol_processor.hpp"

TEST(SymbolProcessor_test, addSymbols) {
  kazoo::SymbolProcessor processor;

  EXPECT_EQ(processor.getNumberOfSymbols(), 0);
  EXPECT_EQ(processor.getNumBits(), 0);
  processor.addSymbol(kazoo::Token::SYMBOL_00);
  EXPECT_EQ(processor.getNumberOfSymbols(), 1);
  EXPECT_EQ(processor.getNumBits(), 2);
}