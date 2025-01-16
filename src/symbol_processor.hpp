/// @file symbol.hpp
/// @author Joshua Jerred (https://joshuajer.red)
/// @brief
/// @date 2025-01-16
/// @copyright Copyright (c) 2025

#pragma once

#include <array>
#include <cstdint>
#include <deque>

namespace kazoo {

enum class Token : uint32_t {
  UNKNOWN = 0,
  SYMBOL_00,
  SYMBOL_01,
  SYMBOL_10,
  SYMBOL_11,
  _SYMBOL_COUNT
};

struct Symbol {
  const Token token;
  const uint32_t value;
  static constexpr uint32_t NUM_BITS = 2;
};

static const std::array<Symbol, static_cast<size_t>(Token::_SYMBOL_COUNT)>
    SYMBOLS = {{{Token::SYMBOL_00, 0},
                {Token::SYMBOL_01, 1},
                {Token::SYMBOL_10, 2},
                {Token::SYMBOL_11, 3}}};

class SymbolProcessor {
public:
  SymbolProcessor() = default;

  size_t getNumberOfSymbols() const { return symbols_.size(); }

  void addSymbol(Token token) {
    if (token == Token::UNKNOWN || token == Token::_SYMBOL_COUNT) {
      throw std::invalid_argument("Invalid token");
    }

    symbols_.push_back(token);
  }

  size_t getNumBits() const { return symbols_.size() * Symbol::NUM_BITS; }

private:
  std::deque<Token> symbols_;
};

} // namespace kazoo