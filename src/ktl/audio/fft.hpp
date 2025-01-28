/// =*= kazooformation =*=
/// @author joshua
/// @date 1/22/25
/// @copyright Copyright (c) 2025

#pragma once

#include <cmath>
#include <complex>
#include <fftw3.h>
#include <span>

#include "audio_channel.hpp"
#include "audio_constants.hpp"

#include <common/assert.hpp>

namespace kazoo {

/// @cite https://stackoverflow.com/a/22059819
class Fft {
 public:
  Fft() = default;

  // static void compute() {
  //   const int N = 64;
  //   std::array<fftw_complex, N> in;
  //   std::array<fftw_complex, N> out;
  //
  //   int i;
  //   prepareCosWave(in);
  //   performFft(in, out);
  //   for (i = 0; i < N; i++)
  //     printf("freq: %3d %+9.5f %+9.5f I\n", i, out[i][0], out[i][1]);
  // }

  //  static void performFft(std::span<fftw_complex> in,
  //                         std::span<fftw_complex> out) {
  //    const int N = in.size();
  //    if (in.size() != out.size()) {
  //      throw std::runtime_error("Input and output arrays must be the same
  //      size");
  //    }
  //
  //    fftw_plan p;
  //    p = fftw_plan_dft_1d(N, in.data(), out.data(), FFTW_FORWARD,
  //    FFTW_ESTIMATE); fftw_execute(p); fftw_destroy_plan(p); fftw_cleanup();
  //
  //    for (int i = 0; i < N; i++)
  //      printf("freq: %3d %+9.5f %+9.5f I\n", i, out[i][0], out[i][1]);
  //  }

  // frequency:amplitude
  struct FftResults {
    /// @brief Frequency/amplitude pair.
    using FreqAmp = std::pair<double, double>;

    /// @brief Vector of frequency/amplitude pairs.
    std::vector<FreqAmp> frequency_amplitude;

    /// @brief Maximum amplitude found in the frequency domain.
    FreqAmp max_amplitude{0.0, 0.0};
    size_t max_amplitude_index = 0;

    void reset() {
      frequency_amplitude.clear();
      max_amplitude = {0.0, 0.0};
      max_amplitude_index = 0;
    }
  };

  /// @todo FFT without floating point?
  /// @param input - A span in PCM format. (16-bit signed integers,
  /// AUDIO_SAMPLE_RATE)
  static void performFftFrequency(const std::span<const int16_t> input,
                                  FftResults& results,
                                  uint32_t fixed_fft_bins = 0) {
    // https://cplusplus.com/forum/beginner/251061/

    const uint32_t num_samples = input.size();
    const uint32_t fft_bins =
        fixed_fft_bins == 0 ? num_samples : fixed_fft_bins;
    // The frequency resolution of the FFT (bin width)
    const double delta_f = static_cast<double>(AUDIO_SAMPLE_RATE) / fft_bins;

    // Load in and normalize the data
    std::vector<double> in;  // Data in the time domain
    for (const auto sample : input) {
      in.push_back(static_cast<double>(sample) / 32768.0);
      // in.push_back(static_cast<double>(a));
    }

    std::vector<std::complex<double>> out(in.size() / 2 + 1);

    fftw_plan p = fftw_plan_dft_r2c_1d(
        in.size(), in.data(), reinterpret_cast<fftw_complex*>(out.data()),
        FFTW_ESTIMATE);

    fftw_execute(p);

    // std::cout << "in/out size: " << in.size() << " " << out.size() << '\n';
    double max_amplitude = 0.0;
    double freq_at_max_amplitude = 0.0;
    size_t max_amplitude_index = 0;
    for (int K = 1; K < out.size(); ++K) {
      const double frequency = K * delta_f;
      const double amplitude = 2 * std::abs(out.at(K)) / in.size();

      if (amplitude > max_amplitude) {
        max_amplitude = amplitude;
        freq_at_max_amplitude = frequency;
        max_amplitude_index = K;
      }

      results.frequency_amplitude.push_back({frequency, amplitude});
    }
    results.max_amplitude = {freq_at_max_amplitude, max_amplitude};
    results.max_amplitude_index = max_amplitude_index;

    fftw_destroy_plan(p);  // destructor
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