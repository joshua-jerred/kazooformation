/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file symbol.hpp
/// @date 2025-01-21

#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>

#include <common/assert.hpp>
#include <ktl/audio/audio_channel.hpp>
#include <ktl/encoder_context.hpp>
#include <ktl/i_symbol_stream.hpp>

namespace kazoo {

class ISymbolModel {
 public:
  virtual ~ISymbolModel() = default;

  virtual size_t getSymbolBitWidth() const = 0;

  virtual uint32_t getSymbolBitMask() const {
    return (1 << getSymbolBitWidth()) - 1U;
  }

  /// @brief Get the value corresponding to the numeric token id.
  /// @param token_id - The Token_t cast to an unsigned integer.
  /// @return uint32_t - The value of the token.
  virtual uint32_t getValue(uint32_t token_id) const = 0;

  /// From a binary value, get the corresponding symbol/token id.
  /// @todo Need to remove 'token' verbiage, use 'symbol.id' and 'symbol.value'
  /// instead. This is for the sake of getting rid of templated interfaces which
  /// was a mistake from 5 hours of effort ago.
  /// @param value - The binary value to convert to a token id.
  /// @return uint32_t - The token id corresponding to the value.
  virtual uint32_t getSymbolId(uint32_t value) const = 0;

  virtual void encodeSymbol(EncoderContext& context, uint32_t token_id,
                            IAudioChannel& audio_channel) const = 0;

  virtual void decodeAudioToSymbols(const IAudioChannel& audio_channel,
                                    ISymbolStream& symbol_stream) const = 0;

  virtual uint32_t getNumSamplesPerSymbol() const = 0;
};

template <typename Token_t>
class SymbolModel : public ISymbolModel {
 public:
  /// @brief Maps a token to it's corresponding binary value.
  using TokenValueMap = std::map<Token_t, uint32_t>;

  SymbolModel(const TokenValueMap token_values, uint32_t symbol_bit_width)
      : token_values_{token_values},
        symbol_bit_width_{symbol_bit_width} {}

  size_t getSymbolBitWidth() const override { return symbol_bit_width_; }

  uint32_t getValue(uint32_t token_id) const override {
    return token_values_.at(static_cast<Token_t>(token_id));
  }

  // uint32_t getValue(Token_t token) const { return token_values_.at(token); }

  Token_t getToken(uint32_t value) const {
    for (const auto& [token, token_value] : token_values_) {
      if (token_value == value) {
        return token;
      }
    }
    KTL_ASSERT(false);
    return static_cast<Token_t>(0);
  }

  uint32_t getSymbolId(uint32_t value) const override {
    return static_cast<uint32_t>(getToken(value));
  }

  void encodeSymbol(EncoderContext& context, uint32_t token_id,
                    IAudioChannel& audio_channel) const override {
    (void)context;
    (void)token_id;
    (void)audio_channel;
    KTL_ASSERT(false);  // This should be implemented by the derived class.
  }

  void decodeAudioToSymbols(const IAudioChannel& audio_channel,
                            ISymbolStream& symbol_stream) const override {
    (void)audio_channel;
    (void)symbol_stream;
    KTL_ASSERT(false);  // This should be implemented by the derived class.
  }

  uint32_t getNumSamplesPerSymbol() const override {
    KTL_ASSERT(false);  // This should be implemented by the derived class.
    return 0;
  }

 private:
  const TokenValueMap token_values_;
  const uint32_t symbol_bit_width_;
};

}  // namespace kazoo