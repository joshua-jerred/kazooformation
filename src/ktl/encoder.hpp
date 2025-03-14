/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file encoder.hpp
/// @date 2025-01-21

#pragma once

#include <span>

#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/kazoo_pcm.hpp>
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
  /// @param pre_post_padding - If true, the encoder will add padding before
  /// and after the symbols to help with synchronization.
  /// @return The number of symbols that were added.
  size_t encodeAvailableSymbols(SymbolStream<Token_t>& symbol_stream,
                                IAudioChannel& audio_channel,
                                bool pre_post_padding = false) {
    if (pre_post_padding) {
      encodePreamble(audio_channel);
    }

    size_t num_symbols_added = 0;
    Token_t symbol_token;
    while (symbol_stream.popSymbol(symbol_token)) {
      symbol_model_.encodeSymbol(context_, static_cast<uint32_t>(symbol_token),
                                 audio_channel);
      ++num_symbols_added;
    }

    if (pre_post_padding) {
      encodeTail(audio_channel);
    }

    return num_symbols_added;
  }

 private:
  /// @brief Adds a preamble to the audio channel to help with synchronization.
  /// @param audio_channel - The audio channel to add the preamble to.
  void encodePreamble(IAudioChannel& audio_channel) {
    // auto encodeSpan = [&audio_channel](std::span<const int16_t> span) {
    //   for (int16_t sample : span) {
    //     audio_channel.addSample(sample);
    //   }
    // };

    // encodeSpan(pcm_data::DOOT2_PCM);
    // encodeSpan(pcm_data::DOOT3_PCM);
    // encodeSpan(pcm_data::DOOT1_PCM);

    // encodeSpan(pcm_data::DOOT4_PCM);
    // encodeSpan(pcm_data::DOOT3_PCM);

    // bit of silence
    static constexpr size_t PREAMBLE_SYMBOLS = 4410UL;  // 44100 / 10;
    const int16_t SILENT_SAMPLE = 0;
    for (size_t i = 0; i < PREAMBLE_SYMBOLS; ++i) {
      audio_channel.addSample(SILENT_SAMPLE);
    }
  }

  /// @todo
  void encodeTail(IAudioChannel& audio_channel) {}

  EncoderContext context_;

  const ISymbolModel& symbol_model_;
};

}  // namespace kazoo