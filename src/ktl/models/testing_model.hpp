/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <common/assert.hpp>
#include <ktl/audio/fft.hpp>
#include <ktl/audio/wave_tools.hpp>
#include <ktl/encoder.hpp>
#include <ktl/models/symbol_model.hpp>
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

  class Model final : public SymbolModel<Token> {
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

    void decodeAudioToSymbols(const IAudioChannel& audio_channel,
                              ISymbolStream& symbol_stream) const override {
      const auto samples = audio_channel.getSamplesRef();
      const size_t num_samples = samples.size();

      Fft::FftResults results;
      for (size_t i = 0; i < num_samples; i += SAMPLES_PER_SYMBOL) {
        std::span<const int16_t> symbol_width_samples{samples.data() + i,
                                                      SAMPLES_PER_SYMBOL};
        Fft::performFftFrequency(symbol_width_samples, results);

        double peak_freq = results.max_amplitude.frequency;
        if (std::abs(peak_freq - SYM_0_FREQ) < MAX_SYM_FREQ_DEVIATION) {
          symbol_stream.addSymbolId(static_cast<uint32_t>(Token::SYMBOL_0));
        } else if (std::abs(peak_freq - SYM_1_FREQ) < MAX_SYM_FREQ_DEVIATION) {
          symbol_stream.addSymbolId(static_cast<uint32_t>(Token::SYMBOL_1));
        } else {
          symbol_stream.addSymbolId(static_cast<uint32_t>(Token::UNKNOWN));
        }
      }
    }
  };

  using Stream = SymbolStream<Token>;
  using Transcoder = Encoder<Token>;

  static constexpr size_t SAMPLES_PER_SYMBOL = 200U;

 private:
  static constexpr double SYM_0_FREQ = 1500.0;
  static constexpr double SYM_1_FREQ = 3000.0;
  static constexpr double MAX_SYM_FREQ_DEVIATION = 800.0;
};

}  // namespace kazoo::model