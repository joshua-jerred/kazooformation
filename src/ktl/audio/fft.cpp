/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file fft.cpp
/// @date 2025-03-13

#include "ktl/audio/fft.hpp"

std::ostream& operator<<(std::ostream& os,
                         const kazoo::Fft::FftResults& results) {
  os << "FFT Results:" << std::endl;
  os << "Max Amplitude [" << results.max_amplitude_index
     << "]: " << results.max_amplitude.amplitude
     << " at frequency: " << results.max_amplitude.frequency << std::endl;
  os << "Average Amplitude: " << results.average_amplitude << std::endl;
  os << "Amplitude Variance: " << results.normalized_amplitude_variance
     << std::endl;
  os << "Amplitude Standard Deviation: " << results.normalized_amplitude_std_dev
     << std::endl;
  // for (const auto& res : results.frequency_amplitude) {
  // std::cout << "freq: " << res.frequency << " amp: " << res.amplitude
  // << std::endl;
  // }

  return os;
}