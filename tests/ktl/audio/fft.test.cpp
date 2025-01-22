
#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/fft.hpp>
#include <ktl/audio/wav_file.hpp>

TEST(Fft_test, basicTest) {
  kazoo::WavFile wav_file;

  size_t NUM_SAMPLES = 300;
  size_t SINE_FREQ_HZ = 500;

  wav_file.addSineWaveSamples(SINE_FREQ_HZ, 0.9, NUM_SAMPLES);
  EXPECT_EQ(wav_file.getNumSamples(), NUM_SAMPLES);
  EXPECT_EQ(wav_file.getSamplesRef().size(), NUM_SAMPLES);

  kazoo::Fft::FftResults results;
  kazoo::Fft::performFftFrequency(wav_file, results);

  static constexpr double threshold = 0.1;

  double max_amplitude = 0.0;
  double freq_at_max_amplitude = 0.0;
  for (const auto& result : results) {
    if (result.second > max_amplitude) {
      max_amplitude = result.second;
      freq_at_max_amplitude = result.first;
    }
    if (std::abs(result.second) > threshold) {
      std::cout << "Frequency " << result.first << " Amplitude "
                << result.second << '\n';
    }
  }

  std::cout << "Max Amplitude: " << max_amplitude << " at frequency "
            << freq_at_max_amplitude << '\n';
}
