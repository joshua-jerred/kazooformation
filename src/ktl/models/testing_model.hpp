/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <common/assert.hpp>
#include <ktl/audio/wave_tools.hpp>
#include <ktl/encoder.hpp>
#include <ktl/symbol_model.hpp>
#include <ktl/symbol_stream.hpp>

namespace kazoo::model {

class Testing {
 public:
  enum class Token : uint32_t {
    UNKNOWN = 0,
    SYMBOL_0,
    SYMBOL_1,
    _SYMBOL_COUNT
  };

  class Model : public SymbolModel<Token> {
   public:
    Model()
        : SymbolModel<Token>{{{Token::SYMBOL_0, 0}, {Token::SYMBOL_1, 1}}, 1} {}

    void encodeSymbol(EncoderContext& context, uint32_t token_id,
                      IAudioChannel& audio_channel) const override {
      switch (static_cast<Token>(token_id)) {
        case Token::SYMBOL_0:
          context.wave_angle = WaveTools::generateSinWaveSamples(
              audio_channel, 1500, 200, 0.5, context.wave_angle);
          break;

        case Token::SYMBOL_1:
          context.wave_angle = WaveTools::generateSinWaveSamples(
              audio_channel, 3000, 200, 0.5, context.wave_angle);
          break;

        default:
          KTL_ASSERT(false);
      };
    }
  };

  static constexpr void decodeSymbols(const IAudioChannel& audio_channel,
                     SymbolStream<Token>& symbol_stream) {
    // This is a test model, so we don't need to decode anything.
  }

  using Stream = SymbolStream<Token>;
  using Transcoder = Encoder<Token>;

  static constexpr size_t SAMPLES_PER_SYMBOL = 200U;
};

}  // namespace kazoo::model