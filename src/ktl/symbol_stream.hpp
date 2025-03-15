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
#include <ktl/i_symbol_stream.hpp>
#include <ktl/models/symbol_model.hpp>
#include <ktl/symbol.hpp>

namespace kazoo {

/// @brief A simple Symbol Stream structure. Interfaces with the transcoder and
/// binary stream.
/// @tparam Token_t - The token enum type
template <typename Token_t>
class SymbolStream : public ISymbolStream {
 public:
  /// @brief Constructor
  /// @param symbol_model - The symbol table
  SymbolStream(const ISymbolModel &symbol_model)
      : symbol_model_(symbol_model) {
  }

  void clear() override {
    symbols_.clear();
  }

  /// @brief Add a symbol to the end of the stream.
  /// @param token - The symbol token to add.
  void addSymbol(Token_t token) {
    symbols_.push_back(token);
  }

  void addSymbolId(uint32_t token_id) override {
    symbols_.push_back(static_cast<Token_t>(token_id));
  }

  /// @brief Get the number of symbols in the stream.
  /// @return size_t The number of symbols
  size_t getNumSymbols() const {
    return symbols_.size();
  }

  size_t getNumBits() const {
    return symbols_.size() * symbol_model_.getSymbolBitWidth();
  }

  size_t getNumBytes() const override {
    return std::ceil(getNumBits() / 8.0);
  }

  bool popSymbol(Token_t &token) {
    if (symbols_.empty()) {
      return false;
    }
    token = symbols_.front();
    symbols_.pop_front();
    return true;
  }

  bool popSymbolTokenId(uint32_t &token_id) override {
    if (symbols_.empty()) {
      return false;
    }
    token_id = static_cast<uint32_t>(symbols_.front());
    symbols_.pop_front();
    return true;
  }

  bool populateBinaryStream(BinaryStream &binary_stream, size_t num_bytes,
                            bool pop_symbols = true) override {
    if (num_bytes > getNumBytes()) {
      return false;
    }

    size_t num_symbols = std::clamp<size_t>(
        std::ceil((num_bytes * 8.0) / symbol_model_.getSymbolBitWidth()), 0U,
        symbols_.size());

    for (size_t i = 0; i < num_symbols; ++i) {
      binary_stream.addBits(
          symbol_model_.getValue(static_cast<uint32_t>(symbols_.at(i))),
          symbol_model_.getSymbolBitWidth());

      if (pop_symbols) {
        symbols_.pop_front();
        num_symbols--;
        i--;
      }
    }

    return true;
  }

  /// @brief Process a binary stream and convert it to symbols, removing the
  /// bits from the binary stream.
  /// @param binary_stream - The binary stream to process
  /// @return std::pair<number of bytes processed, number of symbols processed>
  std::pair<size_t, size_t> processBinaryStream(
      BinaryStream &binary_stream) override {
    KTL_ASSERT(binary_stream.isByteAligned());
    size_t num_bytes = binary_stream.getNumBytes();
    uint8_t symbol_value_buffer{0};
    size_t num_symbols_processed{0};
    while (binary_stream.popBits(symbol_value_buffer,
                                 symbol_model_.getSymbolBitWidth())) {
      symbols_.push_back(
          static_cast<Token_t>(symbol_model_.getSymbolId(symbol_value_buffer)));
      ++num_symbols_processed;
    }

    return {(num_bytes - binary_stream.getNumBytes()), num_symbols_processed};
  }

 private:
  const ISymbolModel &symbol_model_;
  std::deque<Token_t> symbols_{};
};

}  // namespace kazoo
