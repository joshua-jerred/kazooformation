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

// constexpr bool DEBUG_PRINT = false;
// inline void debugPrint(const std::string& str) {
// if constexpr (!DEBUG_PRINT) {
// return;
// }
// std::cout << str << std::endl;
// }

class K2PeakModel {
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
      static constexpr uint32_t NUM_FFTS_PER_SYMBOL = 5;
      constexpr size_t FFT_WINDOW_SIZE =
          SAMPLES_PER_SYMBOL / NUM_FFTS_PER_SYMBOL;
      constexpr size_t HAMMING_NUM_SAMPLES = FFT_WINDOW_SIZE * 0.1;

      constexpr size_t tmp = FFT_WINDOW_SIZE * 6;

      const auto samples = audio_channel.getSamplesRef();
      const size_t num_samples = samples.size();

      size_t sample_index = 0;
      for (size_t i = 0; sample_index < num_samples; i += 1) {
        std::vector<Token> detected_symbols;

        for (size_t j = 0; j < NUM_FFTS_PER_SYMBOL; j++) {
          Fft::FftResults partial_fft_results;

          // Span containing NUM_FFTS_PER_SYMBOL worth of samples, an FFT is
          // performed NUM_FFTS_PER_SYMBOL times per symbol for alignment
          // reasons.

          const size_t start_position =
              sample_index + (j * NUM_FFTS_PER_SYMBOL);
          std::span<const int16_t> window_span{samples.data() + start_position,
                                               FFT_WINDOW_SIZE};

          // Hamming window/filtering
          // std::vector<int16_t> hamming_windowed_samples(FFT_WINDOW_SIZE);
          // for (size_t k = 0; k < FFT_WINDOW_SIZE; k++) {
          //   if (k > HAMMING_NUM_SAMPLES &&
          //       k < (FFT_WINDOW_SIZE - HAMMING_NUM_SAMPLES)) {
          //     hamming_windowed_samples[k] = window_span[k];
          //   } else {
          //     hamming_windowed_samples[k] = static_cast<int16_t>(
          //         window_span[k] *
          //         (0.54 - 0.46 * cos((2 * M_PI * k) / (FFT_WINDOW_SIZE -
          //         1))));
          //   }
          // }

          Fft::performFftFrequency(window_span, partial_fft_results);

          Token symbol = detectSymbolFromFftResults(partial_fft_results);

          // if (DEBUG_PRINT) {
          std::cout << (symbol == Token::SYMBOL_0   ? "0"
                        : symbol == Token::SYMBOL_1 ? "1"
                                                    : "X")
                    << " [" << i << ", " << j << ", " << start_position << "]: "
                    << " pfreq: " << partial_fft_results.max_amplitude.frequency
                    << " max_amp: "
                    << partial_fft_results.max_amplitude.amplitude
                    << " avg_amp: " << partial_fft_results.average_amplitude
                    << " amp_variance: "
                    << partial_fft_results.normalized_amplitude_variance
                    << " amp_sdev: "
                    << partial_fft_results.normalized_amplitude_std_dev
                    << std::endl;
          // }

          sample_index += FFT_WINDOW_SIZE;

          // If the symbol is unknown, just throw the whole sample span away as
          // we're misaligned.
          if (symbol == Token::UNKNOWN) {
            break;
          }

          detected_symbols.push_back(symbol);
        }

        // If we don't have enough symbols to make a decision, we don't add
        // anything.
        if (detected_symbols.size() < NUM_FFTS_PER_SYMBOL - 2) {
          // We didn't get enough symbols to make a decision, so we don't add
          // anything.
          // debugPrint("rejecting symbol");
          continue;
        }

        // Process the detected symbols
        size_t num_0_count = 0;
        size_t num_1_count = 0;
        for (const auto& symbol : detected_symbols) {
          if (symbol == Token::SYMBOL_0) {
            num_0_count++;
          } else if (symbol == Token::SYMBOL_1) {
            num_1_count++;
          }
        }

        if (num_0_count > num_1_count) {
          symbol_stream.addSymbolId(static_cast<uint32_t>(Token::SYMBOL_0));
          // debugPrint("Detected Symbol: 0");
          // std::cout << "Detected Symbol: 0" << std::endl;
        } else if (num_1_count > num_0_count) {
          symbol_stream.addSymbolId(static_cast<uint32_t>(Token::SYMBOL_1));
          // debugPrint("Detected Symbol: 1");
          // std::cout << "Detected Symbol: 1" << std::endl;
        } else {
          // If it's a tie, we don't know what it is.
          // symbol_stream.addSymbolId(static_cast<uint32_t>(Token::UNKNOWN));
          // std::cout << "Detected Symbol: X" << std::endl;
        }

        if (sample_index > num_samples) {
          break;
        }
      }
    }

    uint32_t getNumSamplesPerSymbol() const override {
      return SAMPLES_PER_SYMBOL;
    }

   private:
    Token detectSymbolFromFftResults(const Fft::FftResults& results) const {
      // const double CUT_OFF_AMPLITUDE_STANDARD_DEVIATION = 0.5;

      const double peak_freq = results.max_amplitude.frequency;
      if (peak_freq >= MIN_FREQ_A && peak_freq <= MAX_FREQ_A) {
        return Token::SYMBOL_0;
      } else if (peak_freq >= MIN_FREQ_B && peak_freq <= MAX_FREQ_B) {
        return Token::SYMBOL_1;
      }

      return Token::UNKNOWN;
    }
  };

  using Stream = SymbolStream<Token>;
  using Transcoder = Encoder<Token>;

  static constexpr size_t SAMPLES_PER_SYMBOL = 4410U;

 private:
  static const std::array<int16_t, SAMPLES_PER_SYMBOL> SYMBOL_0_SAMPLES;
  static const std::array<int16_t, SAMPLES_PER_SYMBOL> SYMBOL_1_SAMPLES;

  static constexpr double CENTER_FREQ = 2000.0;

  // Symbol 0
  static constexpr double MIN_FREQ_A = 200.0;
  static constexpr double MAX_FREQ_A = 1200.0;

  // Symbol 1
  static constexpr double MIN_FREQ_B = 1500.0;
  static constexpr double MAX_FREQ_B = 6000.0;
};

}  // namespace kazoo::model