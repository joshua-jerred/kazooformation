
#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/fft.hpp>
#include <ktl/audio/wav_file.hpp>
#include <ktl/audio/wave_tools.hpp>

TEST(Fft_test, basicTest) {
  kazoo::WavFile wav_file;

  constexpr size_t NUM_SAMPLES = 400;
  constexpr size_t SINE_FREQ_HZ = 1000;
  constexpr double AMPLITUDE = 0.9;

  kazoo::WaveTools::generateSinWaveSamples(wav_file, SINE_FREQ_HZ, NUM_SAMPLES,
                                           AMPLITUDE);
  EXPECT_EQ(wav_file.getNumSamples(), NUM_SAMPLES);
  EXPECT_EQ(wav_file.getSamplesRef().size(), NUM_SAMPLES);

  const auto wav_span = wav_file.getSamplesRef();
  kazoo::Fft::FftResults results;
  kazoo::Fft::performFftFrequency(wav_span, results);

  std::cout << "-Max Amplitude @[" << results.max_amplitude_index
            << "]: " << results.max_amplitude.second << " at frequency "
            << results.max_amplitude.first << std::endl;

  EXPECT_NEAR(results.max_amplitude.first, SINE_FREQ_HZ, 10.0);
}
