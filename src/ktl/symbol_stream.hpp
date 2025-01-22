/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file symbol_stream.hpp
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

#include <ktl/binary_stream.hpp>
#include <ktl/symbol.hpp>
#include <ktl/symbol_model.hpp>

namespace kazoo {

/// @brief A simple Symbol Stream structure. Interfaces with the transcoder and
/// binary stream.
/// @tparam Token_t - The token enum type
template <typename Token_t>
class SymbolStream {
 public:
  /// @brief Constructor
  /// @param symbol_model - The symbol table
  SymbolStream(const ISymbolModel &symbol_model)
      : symbol_model_(symbol_model) {}

  /// @brief Add a symbol to the end of the stream.
  /// @param token - The symbol token to add.
  void addSymbol(Token_t token) { symbols_.push_back(token); }

  /// @brief Get the number of symbols in the stream.
  /// @return size_t The number of symbols
  size_t getNumSymbols() const { return symbols_.size(); }

  size_t getNumBits() const {
    return symbols_.size() * symbol_model_.getSymbolBitWidth();
  }

  size_t getNumBytes() const { return std::ceil(getNumBits() / 8.0); }

  bool popSymbol(Token_t &token) {
    if (symbols_.empty()) {
      return false;
    }
    token = symbols_.front();
    symbols_.pop_front();
    return true;
  }

  bool popSymbolTokenId(uint32_t &token_id) {
    if (symbols_.empty()) {
      return false;
    }
    token_id = static_cast<uint32_t>(symbols_.front());
    symbols_.pop_front();
    return true;
  }

  bool populateBinaryStream(BinaryStream &binary_stream, size_t num_bytes,
                            bool pop_bytes = true) {
    if (num_bytes > getNumBytes()) {
      return false;
    }

    const size_t num_symbols = std::clamp<size_t>(
        std::ceil((num_bytes * 8.0) / symbol_model_.getSymbolBitWidth()), 0U,
        symbols_.size());

    for (size_t i = 0; i < num_symbols; ++i) {
      binary_stream.addBits(
          symbol_model_.getValue(static_cast<uint32_t>(symbols_.at(i))),
          symbol_model_.getSymbolBitWidth());

      if (pop_bytes) {
        symbols_.pop_front();
      }
    }

    return true;
  }

  /// @brief Process a binary stream and convert it to symbols.
  /// @param binary_stream - The binary stream to process
  /// @param pop_bytes - Whether to pop bytes from the input stream
  /// @return std::pair<number of bytes processed, number of symbols processed>
  std::pair<size_t, size_t> processBinaryStream(BinaryStream &binary_stream,
                                                bool pop_bytes = true) {
    const auto bytes = binary_stream.getStreamDataConst();
    size_t num_bytes_processed = 0;

    const size_t symbol_bit_width = symbol_model_.getSymbolBitWidth();
    const size_t num_symbols = bytes.size() * 8 / symbol_bit_width;
    for (size_t i = 0; i < num_symbols; ++i) {
      uint32_t symbol_value = 0;
      for (size_t j = 0; j < symbol_bit_width; ++j) {
        const size_t byte_index = (i * symbol_bit_width + j) / 8;
        const size_t bit_index = (i * symbol_bit_width + j) % 8;
        const uint8_t byte = bytes.at(byte_index);
        const uint8_t bit = (byte >> (7 - bit_index)) & 0x1;
        symbol_value |= bit << (symbol_bit_width - 1 - j);
      }

      symbols_.push_back(
          static_cast<Token_t>(symbol_model_.getSymbolId(symbol_value)));
      ++num_bytes_processed;
    }

    return {num_bytes_processed, num_symbols};
  }

 private:
  const ISymbolModel &symbol_model_;
  std::deque<Token_t> symbols_{};
};

}  // namespace kazoo
