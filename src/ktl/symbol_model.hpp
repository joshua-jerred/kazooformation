/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file symbol.hpp
/// @date 2025-01-21

#pragma once

#include <cstddef>
#include <cstdint>
#include <map>

#include <ktl/audio_channel.hpp>
#include <ktl/symbol.hpp>

namespace kazoo {

template <typename Token_t>
class ISymbolModel {
 public:
  virtual ~ISymbolModel() = default;

  virtual size_t getSymbolBitWidth() const = 0;

  virtual uint32_t getSymbolBitMask() const {
    return (1 << getSymbolBitWidth()) - 1U;
  }

  virtual uint32_t getValue(Token_t) const = 0;
};

template <typename Token_t>
class SymbolModel : public ISymbolModel<Token_t> {
 public:
  using TokenValueMap = std::map<Token_t, uint32_t>;

  SymbolModel(const TokenValueMap token_values, uint32_t symbol_bit_width)
      : token_values_{token_values}, symbol_bit_width_{symbol_bit_width} {}

  size_t getSymbolBitWidth() const override { return symbol_bit_width_; }

  uint32_t getValue(Token_t token) const override {
    return token_values_.at(token);
  }

 private:
  const TokenValueMap token_values_;
  const uint32_t symbol_bit_width_;
};

}  // namespace kazoo