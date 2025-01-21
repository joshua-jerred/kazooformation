/// @file symbol.hpp
/// @author Joshua Jerred (https://joshuajer.red)
/// @brief
/// @date 2025-01-16
/// @copyright Copyright (c) 2025

#pragma once

#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <deque>
#include <map>
#include <sstream>
#include <vector>

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
  static constexpr uint32_t BIT_MASK = 0b11;
};

static const std::map<Token, Symbol> SYMBOLS = {
    {{Token::SYMBOL_00, {Token::SYMBOL_00, 0}},
     {Token::SYMBOL_01, {Token::SYMBOL_01, 1}},
     {Token::SYMBOL_10, {Token::SYMBOL_10, 2}},
     {Token::SYMBOL_11, {Token::SYMBOL_11, 3}}}};

class SymbolStream {
public:
  SymbolStream() = default;

  void addSymbol(Token token) {
    if (token == Token::UNKNOWN || token == Token::_SYMBOL_COUNT) {
      throw std::invalid_argument("Invalid token");
    }

    symbols_.push_back(token);
  }

  size_t getNumBits() const { return symbols_.size() * Symbol::NUM_BITS; }

  size_t getNumBytes() const { return std::ceil(getNumBits() / 8.0); }

  size_t getNumSymbols() const { return symbols_.size(); }

  bool isByteAligned() const { return getNumBits() % 8 == 0; }

  /// @brief Get the number of unspecified bits in the last byte.
  /// @details For example, if 5x symbols are in the stream, and the symbol bit
  /// width is 2, there will be 1 full byte followed by 2 extra bits.
  /// @return The number of unspecified bits in the last byte of the buffer.
  /// This will be a value between 0 and 7.
  size_t getNumUpspecifiedBits() const {
    if (isByteAligned()) {
      return 0;
    }

    return 8 - (getNumBits() % 8);
  }

  /// @brief Get the next num_bytes bytes from the symbol processor.
  /// @details Bit order is big-endian.
  /// ex:  addSymbol(11), addSymbol(00), addSymbol(00), addSymbol(10), ...
  ///      > [0b11000010, ...]
  /// @param[out] bytes - The bytes to be returned
  /// @param num_bytes - The number of bytes to get
  /// @return \c true if the number of bytes was successfully retrieved, \c
  /// false otherwise.
  bool getBytes(std::vector<uint8_t> &bytes, size_t num_bytes) const {
    if (num_bytes > getNumBytes()) {
      return false;
    }

    bytes.clear();

    size_t num_symbols = num_bytes * 8 / Symbol::NUM_BITS;
    const size_t hanging_symbols = (num_symbols - symbols_.size());
    const size_t hanging_bits = hanging_symbols * Symbol::NUM_BITS;
    num_symbols -= hanging_symbols;
    if (hanging_bits > 7) {
      throw std::runtime_error("hanging bits failed");
    }
    std::cout << "hanging bits: " << static_cast<int>(hanging_bits)
              << ", hanging_symbols: " << static_cast<int>(hanging_symbols)
              << std::endl;

    // std::cout << "num symbols: " << num_symbols << std::endl;
    uint8_t byte_buffer = 0;
    uint8_t num_bits_in_byte = 0;
    for (size_t i = 0; i < num_symbols; ++i) {
      static_assert(Symbol::NUM_BITS < 8, "Symbol::NUM_BITS must be < 8");

      const uint8_t shift_amount = 8 - Symbol::NUM_BITS - num_bits_in_byte;
      const uint8_t symbol_value = SYMBOLS.at(symbols_.at(i)).value;
      // std::cout << "shift_amount: " << static_cast<int>(shift_amount)
      //           << ", symbol_value: " << static_cast<int>(symbol_value)
      //           << std::endl;

      byte_buffer |= (symbol_value & static_cast<uint8_t>(Symbol::BIT_MASK))
                     << shift_amount;
      num_bits_in_byte += Symbol::NUM_BITS;

      if (num_bits_in_byte == 8) {
        bytes.push_back(byte_buffer);
        byte_buffer = 0;
        num_bits_in_byte = 0;
      }
      // std::cout << "byte: " << static_cast<int>(byte_buffer) << std::endl;
    }

    if (num_bits_in_byte > 0) {
      bytes.push_back(byte_buffer);
      // std::cout << "added hanging bits" << std::endl;
    }

    return true;
  }

private:
  std::deque<Token> symbols_;
};

} // namespace kazoo

std::ostream &operator<<(std::ostream &os,
                         const kazoo::SymbolStream &symbol_processor) {

  static constexpr bool HEX_MODE = false;
  static constexpr size_t DISPLAY_BYTE_WIDTH = 4;
  static constexpr size_t MAX_BYTES = 64;

  std::vector<uint8_t> byte_data_buffer{};
  assert(symbol_processor.getBytes(
      byte_data_buffer,
      std::clamp<size_t>(symbol_processor.getNumBytes(), 0, MAX_BYTES)));

  size_t bytes_on_line = 0;
  size_t byte_index = 0;
  for (const auto byte : byte_data_buffer) {
    const bool is_last_byte = (byte_data_buffer.size() - byte_index - 1) == 0;

    if (HEX_MODE) {
      os << "|0x" << std::hex << static_cast<int>(byte);
    } else {
      os << "|0b";

      for (int i = 7; i >= 0; i--) {
        if (is_last_byte &&
            (8 - static_cast<int>(symbol_processor.getNumUpspecifiedBits())) <
                i) {
          os << "x";
        } else {
          os << ((byte >> i) & 1U) ? "1" : "0";
        }
      }
    }
    bytes_on_line++;

    if (bytes_on_line >= DISPLAY_BYTE_WIDTH) {
      os << "|" << std::endl;
    }

    byte_index++;
  }

  os << "|";

  return os;
}