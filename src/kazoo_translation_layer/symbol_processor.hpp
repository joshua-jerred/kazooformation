/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file symbol_processor.hpp
/// @date 2025-01-21

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <deque>
#include <map>
#include <sstream>
#include <vector>

#include "binary_stream.hpp"

#include <kazoo_translation_layer/symbol.hpp>
#include <kazoo_translation_layer/symbol_table.hpp>

namespace kazoo {

template <typename Token_t>
class SymbolStream {
 public:
  SymbolStream(const ISymbolTable<Token_t> &symbol_table)
      : symbol_table_(symbol_table) {}

  void addSymbol(Token_t token) { symbols_.push_back(token); }
  size_t getNumSymbols() const { return symbols_.size(); }

  size_t getNumBits() const {
    return symbols_.size() * symbol_table_.getSymbolBitWidth();
  }

  size_t getNumBytes() const { return std::ceil(getNumBits() / 8.0); }

  bool populateBinaryStream(BinaryStream &binary_stream, size_t num_bytes,
                            bool pop_bytes = true) {
    if (num_bytes > getNumBytes()) {
      return false;
    }

    size_t num_symbols = std::clamp<size_t>(
        std::ceil((num_bytes * 8.0) / symbol_table_.getSymbolBitWidth()), 0U,
        symbols_.size());

    for (size_t i = 0; i < num_symbols; ++i) {
      binary_stream.addBits(symbol_table_.getValue(symbols_.at(i)),
                            symbol_table_.getSymbolBitWidth());

      if (pop_bytes) {
        symbols_.pop_front();
      }
    }

    return true;
  }

 private:
  const ISymbolTable<Token_t> &symbol_table_;
  std::deque<Token_t> symbols_{};
};

}  // namespace kazoo
