/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file k1_model.hpp
/// @date 2025-02-02

#pragma once

#include <common/assert.hpp>
#include <ktl/audio/fft.hpp>
#include <ktl/audio/wave_tools.hpp>
#include <ktl/encoder.hpp>
#include <ktl/models/symbol_model.hpp>
#include <ktl/symbol_stream.hpp>

namespace kazoo::model {

class K1Model {
 public:
  enum class Token : uint32_t {
    UNKNOWN = 0,
    SYMBOL_0,
    SYMBOL_1,
    K1_SYMBOL_COUNT
  };

  class Model final : public SymbolModel<Token> {
   public:
    Model()
        : SymbolModel<Token>{{{Token::SYMBOL_0, 0}, {Token::SYMBOL_1, 1}}, 1} {}

    void encodeSymbol(EncoderContext& context, uint32_t token_id,
                      IAudioChannel& audio_channel) const override {
      auto addSamples =
          [&audio_channel](
              const std::array<int16_t, SAMPLES_PER_SYMBOL>& samples) {
            for (auto sample : samples) {
              audio_channel.addSample(sample);
            }
          };

      switch (static_cast<Token>(token_id)) {
        case Token::SYMBOL_0:
          addSamples(SYMBOL_0_SAMPLES);
          break;

        case Token::SYMBOL_1:
          addSamples(SYMBOL_1_SAMPLES);
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

        double peak_freq = results.max_amplitude.first;
        if (peak_freq < CENTER_FREQ) {
          symbol_stream.addSymbolId(static_cast<uint32_t>(Token::SYMBOL_0));
        } else {
          symbol_stream.addSymbolId(static_cast<uint32_t>(Token::SYMBOL_1));
        }
      }
    }
  };

  using Stream = SymbolStream<Token>;
  using Transcoder = Encoder<Token>;

  static constexpr size_t SAMPLES_PER_SYMBOL = 4410U;

 private:
  static const std::array<int16_t, SAMPLES_PER_SYMBOL> SYMBOL_0_SAMPLES;
  static const std::array<int16_t, SAMPLES_PER_SYMBOL> SYMBOL_1_SAMPLES;

  static constexpr double CENTER_FREQ = 2000.0;
};

}  // namespace kazoo::model