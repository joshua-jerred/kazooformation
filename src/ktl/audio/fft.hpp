/// =*= kazooformation =*=
/// @author joshua
/// @date 1/22/25
/// @copyright Copyright (c) 2025

#pragma once

#include <cmath>
#include <complex>
#include <fftw3.h>
#include <fstream>
#include <iostream>
#include <span>

#include "audio_channel.hpp"
#include "audio_constants.hpp"

#include <common/assert.hpp>

namespace kazoo {

/// @cite https://stackoverflow.com/a/22059819
class Fft {
 public:
  Fft() = default;

  // frequency:amplitude
  struct FftResults {
    /// @brief Frequency/amplitude pair.
    struct FreqAmp {
      double frequency;
      // double amplitude_not_normalized;
      double amplitude;
    };

    /// @brief Vector of frequency/amplitude pairs.
    std::vector<FreqAmp> frequency_amplitude;

    /// @brief Maximum amplitude found in the frequency domain.
    FreqAmp max_amplitude{0.0, 0.0};
    /// @brief The index into frequency_amplitude of the maximum amplitude.
    size_t max_amplitude_index = 0;

    /// @brief The average amplitude of the frequency domain.
    double average_amplitude = 0.0;
    double normalized_amplitude_variance = 0.0;
    double normalized_amplitude_std_dev = 0.0;

    // -- normalized amplitude results -- //

    /// @brief Normalized frequency/amplitude pairs, where the amplitude is
    /// normalized to the maximum amplitude with a range of 0.0 to 1.0.
    std::vector<FreqAmp> normalized_frequency_amplitude;

    void reset() {
      frequency_amplitude.clear();
      max_amplitude = {0.0, 0.0};
      max_amplitude_index = 0;
      average_amplitude = 0.0;
      normalized_amplitude_variance = 0.0;
      normalized_amplitude_std_dev = 0.0;
      normalized_frequency_amplitude.clear();
    }

    void saveResultsToCsvFile(const std::string& filename) const {
      std::ofstream file(filename);
      if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " +
                                 filename);
      }

      file << "Frequency,Amplitude" << std::endl;
      for (const auto& res : frequency_amplitude) {
        file << res.frequency << "," << res.amplitude << std::endl;
      }
    }
  };

  /// @todo FFT without floating point?
  /// @param input - A span in PCM format. (16-bit signed integers,
  /// AUDIO_SAMPLE_RATE)
  static void performFftFrequency(const std::span<const int16_t> input,
                                  FftResults& results,
                                  uint32_t fixed_fft_bins = 0) {
    // https://cplusplus.com/forum/beginner/251061/
    // https://dsp.stackexchange.com/questions/19899/how-to-determine-snr-from-fft-of-measured-data
    // https://download.ni.com/evaluation/pxi/Understanding%20FFTs%20and%20Windowing.pdf
    results.reset();

    const uint32_t num_samples = input.size();
    const uint32_t fft_bins =
        fixed_fft_bins == 0 ? num_samples : fixed_fft_bins;

    // The frequency resolution of the FFT (bin width)
    const double delta_f = static_cast<double>(AUDIO_SAMPLE_RATE) / fft_bins;

    // Load in and normalize the data
    std::vector<double> in;  // Data in the time domain
    for (const auto sample : input) {
      in.push_back(static_cast<double>(sample) / 32768.0);
    }

    // Output is n / 2 + 1 complex numbers per
    // https://www.fftw.org/doc/One_002dDimensional-DFTs-of-Real-Data.html
    std::vector<std::complex<double>> out(in.size() / 2 + 1);

    // Perform the FFT
    fftw_plan fft_plan = fftw_plan_dft_r2c_1d(
        in.size(), in.data(), reinterpret_cast<fftw_complex*>(out.data()),
        FFTW_ESTIMATE);
    fftw_execute(fft_plan);
    fftw_destroy_plan(fft_plan);

    double max_amplitude = 0.0;
    double freq_at_max_amplitude = 0.0;
    size_t max_amplitude_index = 0;
    double average_amplitude = 0.0;
    for (int K = 1; K < out.size(); ++K) {
      const double frequency = K * delta_f;
      const double amplitude = std::abs(out[K]) * 2.0 / num_samples;

      if (amplitude > max_amplitude) {
        max_amplitude = amplitude;
        freq_at_max_amplitude = frequency;
        max_amplitude_index = K;
      }

      average_amplitude += amplitude;
      results.frequency_amplitude.push_back({frequency, amplitude});
    }
    results.max_amplitude = {freq_at_max_amplitude, max_amplitude};
    results.max_amplitude_index = max_amplitude_index;
    results.average_amplitude = average_amplitude / out.size();

    // Normalize the amplitudes to the maximum amplitude
    for (auto& res : results.frequency_amplitude) {
      results.normalized_frequency_amplitude.push_back(res);
    }

    KTL_ASSERT(results.normalized_frequency_amplitude.size() ==
               results.frequency_amplitude.size());

    // Calculate the variance/standard deviation of the amplitudes and normalize
    // the amplitudes
    {
      double normalized_variance = 0.0;
      for (auto& res : results.normalized_frequency_amplitude) {
        normalized_variance +=
            std::pow(res.amplitude - results.average_amplitude, 2);
      }
      normalized_variance /= results.normalized_frequency_amplitude.size();

      results.normalized_amplitude_variance = normalized_variance;
      results.normalized_amplitude_std_dev = std::sqrt(normalized_variance);
    }
  }

  static void prepareCosWave(std::span<fftw_complex> in) {
    const size_t N = in.size();
    for (size_t i = 0; i < N; i++) {
      in[i][0] = cos(4 * 2 * M_PI * i / N);
      in[i][1] = 0;  // no imaginary part
    }
  }
};

}  // namespace kazoo

std::ostream& operator<<(std::ostream& os,
                         const kazoo::Fft::FftResults& results);