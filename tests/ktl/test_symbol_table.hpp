/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file test_symbol_table.hpp
/// @date 2025-01-21

#pragma once

#include <ktl/symbol_table.hpp>

enum class TestToken : uint32_t {
  UNKNOWN = 0,
  SYMBOL_00,
  SYMBOL_01,
  SYMBOL_10,
  SYMBOL_11,
  _SYMBOL_COUNT
};

static const kazoo::SymbolTable<TestToken> TEST_SYMBOL_TABLE{
    {{TestToken::SYMBOL_00, 0},
     {TestToken::SYMBOL_01, 1},
     {TestToken::SYMBOL_10, 2},
     {TestToken::SYMBOL_11, 3}},
    2};