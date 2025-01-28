/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file encoder.hpp
/// @date 2025-01-21

#pragma once

#include <ktl/audio/audio_channel.hpp>
#include <ktl/encoder_context.hpp>
#include <ktl/symbol_stream.hpp>

namespace kazoo {

template <typename Token_t>
class Encoder {
 public:
  Encoder(const ISymbolModel& symbol_model)
      : symbol_model_(symbol_model) {}

  /// @brief Pops symbols from the front of the symbol stream and encodes them
  /// into the audio buffer.
  /// @param symbol_stream - The symbol stream to pop symbols off of.
  /// @return The number of symbols that were added.
  size_t encodeAvailableSymbols(SymbolStream<Token_t>& symbol_stream,
                                IAudioChannel& audio_channel) {
    size_t num_symbols_added = 0;
    Token_t symbol_token;
    while (symbol_stream.popSymbol(symbol_token)) {
      symbol_model_.encodeSymbol(context_, static_cast<uint32_t>(symbol_token),
                                 audio_channel);
      ++num_symbols_added;
    }
    return num_symbols_added;
  }

 private:
  EncoderContext context_;

  const ISymbolModel& symbol_model_;
};

}  // namespace kazoo