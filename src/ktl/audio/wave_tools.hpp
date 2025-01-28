/// =*= kazooformation =*=
/// @author joshua
/// @date 1/28/25
/// @copyright Copyright (c) 2025

#pragma once

#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/audio_constants.hpp>

namespace kazoo {

class WaveTools {
 public:
  /// @brief Add a sine wave to the audio channel.
  /// @param channel - The audio channel to add the sine wave to.
  /// @param frequency - The frequency of the sine wave in Hz.
  /// @param num_samples - The number of samples to generate.
  /// @param amplitude - The amplitude of the sine wave, 0.0 to 1.0. Default
  /// is 1.0.
  /// @param starting_angle_radians - The starting angle of the sine wave in
  /// radians.
  /// @return The ending angle of the sine wave in radians.
  static constexpr double generateSinWaveSamples(
      IAudioChannel &channel, const double frequency, const size_t num_samples,
      const double amplitude = 1.0, const double starting_angle_radians = 0.0) {
    // The amount to increment the wave angle by for each sample
    const double wave_angle_delta =
        M_PI * frequency / static_cast<double>(AUDIO_SAMPLE_RATE);

    // The current angle of the wave
    double wave_angle = starting_angle_radians;

    for (size_t i = 0; i < num_samples; i++) {  // For each sample
      wave_angle += wave_angle_delta;
      const int16_t sample = static_cast<int16_t>(
          (amplitude * sin(wave_angle)) * MAX_SAMPLE_AMPLITUDE);
      channel.addSample(sample);
      if (wave_angle > M_PI * 2.0) {
        wave_angle -= M_PI * 2.0;
      }
    }

    return wave_angle;
  }
};

}  // namespace kazoo