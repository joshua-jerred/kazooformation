/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file k1_model.hpp
/// @date 2025-02-02

#pragma once

#include <algorithm>

#include <common/assert.hpp>
#include <ktl/audio/fft.hpp>
#include <ktl/audio/kazoo_pcm.hpp>
#include <ktl/audio/wave_tools.hpp>
#include <ktl/encoder.hpp>
#include <ktl/models/symbol_model.hpp>
#include <ktl/symbol_stream.hpp>

// #define K3_DEBUG_PRINT

namespace kazoo::model {

class K3ReasonableModel {
 public:
  enum class Token : uint32_t {
    UNKNOWN = 0,
    SYMBOL_0,
    SYMBOL_1,
    K3_SYMBOL_COUNT
  };

  static constexpr size_t SYMBOL_BIT_WIDTH = 1;

  class K3Model final : public SymbolModel<Token> {
   public:
    K3Model()
        : SymbolModel<Token>{{{Token::SYMBOL_0, 0b0}, {Token::SYMBOL_1, 0b1}},
                             SYMBOL_BIT_WIDTH} {
    }

    void encodeSymbol(EncoderContext& context, uint32_t token_id,
                      IAudioChannel& audio_channel) const override {
      auto addSamples =
          [&audio_channel](const std::array<int16_t, SAMPLES_PER_SYMBOL>& samples) {
            for (auto sample : samples) {
              audio_channel.addSample(sample);
            }
          };

      switch (static_cast<Token>(token_id)) {
        case Token::SYMBOL_0:
          addSamples(pcm_data::K3_DOOT_LOW);
          break;

        case Token::SYMBOL_1:
          addSamples(pcm_data::K3_DOOT_HIGH);
          break;

        default:
          KTL_ASSERT(false);
      };
    }

    void decodeAudioToSymbols(const IAudioChannel& audio_channel,
                              ISymbolStream& symbol_stream) const override {
      static constexpr uint32_t NUM_FFTS_PER_SYMBOL = 7;
      constexpr size_t FFT_WINDOW_SIZE = SAMPLES_PER_SYMBOL / NUM_FFTS_PER_SYMBOL;
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
          const size_t start_position = sample_index + (j * NUM_FFTS_PER_SYMBOL);
          std::span<const int16_t> window_span{samples.data() + start_position,
                                               FFT_WINDOW_SIZE};

          // Hamming window/filtering
          // std::vector<int16_t> hamming_windowed_samples(FFT_WINDOW_SIZE);
          // for (size_t k = 0; k < FFT_WINDOW_SIZE; k++) {
          //   if (k > HAMMING_NUM_SAMPLES &&
          //       k < (FFT_WINDOW_SIZE - HAMMING_NUM_SAMPLES)) {
          //     hamming_windowed_samples[k] = window_span[k];
          //   } else {
          /// @todo where did these magic numbers come from????
          //     hamming_windowed_samples[k] = static_cast<int16_t>(
          //         window_span[k] *
          //         (0.54 - 0.46 * cos((2 * M_PI * k) / (FFT_WINDOW_SIZE -
          //         1))));
          //   }
          // }

          static constexpr double MIN_PEAK_FREQ_CUT_OFF = 1300.0;
          static constexpr double MAX_PEAK_FREQ_CUT_OFF = 4000.0;
          Fft::performFftFrequency(window_span, partial_fft_results, 0,
                                   MIN_PEAK_FREQ_CUT_OFF, MAX_PEAK_FREQ_CUT_OFF);

          Token symbol = detectSymbolFromFftResults(partial_fft_results);

#ifdef K3_DEBUG_PRINT
          std::cout << (symbol == Token::SYMBOL_0   ? "0"
                        : symbol == Token::SYMBOL_1 ? "1"
                                                    : "XX")
                    << " [" << i << ", " << j << ", " << start_position << "]: "
                    << " pfreq: " << partial_fft_results.max_amplitude.frequency
                    << " max_amp: " << partial_fft_results.max_amplitude.amplitude
                    << " avg_amp: " << partial_fft_results.average_amplitude
                    << " amp_variance: "
                    << partial_fft_results.normalized_amplitude_variance
                    << " amp_sdev: " << partial_fft_results.normalized_amplitude_std_dev
                    << std::endl;
#endif

          sample_index += FFT_WINDOW_SIZE;

          // If the symbol is unknown, just throw the whole sample span away as
          // we're misaligned.
          if (symbol == Token::UNKNOWN) {
            break;
          }

          detected_symbols.push_back(symbol);
        }

        // If we don't have enough consecutive samples to make a decision, we don't add
        // anything.
        /// @todo magic number/parameterize
        if (detected_symbols.size() < NUM_FFTS_PER_SYMBOL - 2) {
          // debugPrint("rejecting symbol");
          continue;
        }
#ifdef K3_DEBUG_PRINT
        std::cout << std::endl;
#endif

        // Process the detected symbols
        // pair<Token, count>
        std::map<Token, size_t> symbol_counts{{Token::SYMBOL_0, 0}, {Token::SYMBOL_1, 0}};

        for (const auto& symbol : detected_symbols) {
          symbol_counts.at(symbol)++;
        }

        // Find the most common symbol
        Token most_common_symbol = Token::UNKNOWN;
        size_t max_count = 0;
        for (const auto& pair : symbol_counts) {
          if (pair.second > max_count) {
            max_count = pair.second;
            most_common_symbol = pair.first;
          }
        }
        if (most_common_symbol != Token::UNKNOWN) {
          symbol_stream.addSymbolId(static_cast<uint32_t>(most_common_symbol));
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
      std::vector<Fft::FftResults::FreqAmp> sorted_peaks = results.getNormalizedPeaks(
          NUM_PEAK_FREQS_TO_CHECK, MIN_AMPLITUDE_CUT_OFF_PARAMETER, MIN_FREQ_CUT_OFF,
          MAX_FREQ_CUT_OFF);

      // std::cout << "f,A: ";
      // for (const auto& peak : sorted_peaks) {
      //   std::cout << "[" << peak.frequency << ", " << peak.amplitude << "]";
      // }
      // std::cout << std::endl;

      // size_t num_0_peaks_of_interest = 0;
      // size_t num_1_peaks_of_interest = 0;
      double sym_0_poi_power = 0;
      double sym_1_poi_power = 0;

      // Start at a non-zero for the sake of a sensible SNR value later on.
      // SNR would be better calculated by taking the 'integral' (summation, discrete
      // probability here) of the whole FFT and then subtracting the peaks of interest.
      double power_outside_poi = 0.2;

      for (const auto& res : sorted_peaks) {
        const double peak_to_check = res.frequency;
        bool peak_used_by_symbol = false;

        for (double peak_of_interest : SYM_A_00_PEAKS_OF_INTEREST) {
          if (std::abs(peak_to_check - peak_of_interest) < PEAK_OF_INTEREST_ENVELOPE_HZ) {
            sym_0_poi_power += res.amplitude;
            peak_used_by_symbol = true;
          }
        }
        for (double peak_of_interest : SYM_D_01_PEAKS_OF_INTEREST) {
          if (std::abs(peak_to_check - peak_of_interest) < PEAK_OF_INTEREST_ENVELOPE_HZ) {
            sym_1_poi_power += res.amplitude;
            peak_used_by_symbol = true;
          }
        }

        if (!peak_used_by_symbol) {
          power_outside_poi += res.amplitude;
        }
      }
      sym_0_poi_power /= NUM_PEAK_FREQS_TO_CHECK;
      sym_1_poi_power /= NUM_PEAK_FREQS_TO_CHECK;
      power_outside_poi /= NUM_PEAK_FREQS_TO_CHECK;

      const double average_working_power =
          (sym_0_poi_power + sym_1_poi_power) / NUM_PEAK_FREQS_TO_CHECK;

      const double signal_to_noise_ratio = average_working_power / power_outside_poi;

      if (average_working_power < MIN_AMPLITUDE_CUT_OFF_PARAMETER) {
        return Token::UNKNOWN;
      }

      std::vector<std::pair<Token, double>> symbol_powers{
          {Token::SYMBOL_0, sym_0_poi_power},
          {Token::SYMBOL_1, sym_1_poi_power}};
      std::sort(symbol_powers.begin(), symbol_powers.end(),
                [](const auto& a, const auto& b) { return a.second > b.second; });

#ifdef K3_DEBUG_PRINT
      std::cout << "snr: " << signal_to_noise_ratio << " - ";
      for (const auto& sym : symbol_powers) {
        std::cout << (sym.first == Token::SYMBOL_0   ? "00"
                      : sym.first == Token::SYMBOL_1 ? "01"
                                                     : "XX")
                  << ":" << sym.second << ", ";
      }
      std::cout << std::endl;
#endif

      if (symbol_powers[0].second >
          symbol_powers[1].second + MIN_AMPLITUDE_CUT_OFF_PARAMETER) {
        return symbol_powers[0].first;
      } else {
        return Token::UNKNOWN;
      }
    }
  };

  using K3Stream = SymbolStream<Token>;
  using K3Transcoder = Encoder<Token>;

  static constexpr size_t SAMPLES_PER_SYMBOL = 4410U;

 private:
  static constexpr size_t NUM_PEAK_FREQS_TO_CHECK = 4;
  static constexpr double PEAK_OF_INTEREST_ENVELOPE_HZ = 50.0;
  static constexpr double MIN_FREQ_CUT_OFF = 1250;
  static constexpr double MAX_FREQ_CUT_OFF = 4000;
  static constexpr double MIN_AMPLITUDE_CUT_OFF_PARAMETER = 0.1;

  // peaks of interest
  static constexpr size_t NUM_PEAKS_OF_INTEREST = 3;
  static constexpr std::array<double, NUM_PEAKS_OF_INTEREST> SYM_A_00_PEAKS_OF_INTEREST{
      1400.0, 1400.0, 1400.0};  // lazy, it's just 1400
  static constexpr std::array<double, NUM_PEAKS_OF_INTEREST> SYM_D_01_PEAKS_OF_INTEREST{
      3220.0, 1610.0,
      3220.0};  // this symbol should be replaced, it's partial FFTs aren't consistent.
};

}  // namespace kazoo::model