
#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/fft.hpp>
#include <ktl/audio/wav_file.hpp>
#include <ktl/audio/wave_tools.hpp>

TEST(Fft_test, basicTest) {
  const std::string RESULTS_CSV_FILE = "Fft_test.basicTest.fft_results.csv";

  // Delete the file if it exists
  if (std::filesystem::exists(RESULTS_CSV_FILE)) {
    std::filesystem::remove(RESULTS_CSV_FILE);
  }
  ASSERT_FALSE(std::filesystem::exists(RESULTS_CSV_FILE));

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

  // std::cout << results;
  results.saveResultsToCsvFile(RESULTS_CSV_FILE);

  constexpr double FREQ_EPSILON = 10.0;
  EXPECT_NEAR(results.max_amplitude.frequency, SINE_FREQ_HZ, FREQ_EPSILON);

  constexpr double AMP_EPSILON = 0.1;
  EXPECT_NEAR(results.max_amplitude.amplitude, AMPLITUDE, AMP_EPSILON);
}

TEST(Fft_test, whiteNoiseTest) {
  const std::string RESULTS_CSV_FILE =
      "Fft_test.whiteNoiseTest.fft_results.csv";

  // Delete the file if it exists
  if (std::filesystem::exists(RESULTS_CSV_FILE)) {
    std::filesystem::remove(RESULTS_CSV_FILE);
  }
  ASSERT_FALSE(std::filesystem::exists(RESULTS_CSV_FILE));

  kazoo::WavFile wav_file;

  constexpr double GAUSSIAN_MEAN = 0.0;
  constexpr double GAUSSIAN_STD_DEV = 0.1;
  constexpr size_t NUM_SAMPLES = 400;

  kazoo::WaveTools::addGaussianNoise(wav_file, GAUSSIAN_MEAN, GAUSSIAN_STD_DEV,
                                     NUM_SAMPLES);
  EXPECT_EQ(wav_file.getNumSamples(), NUM_SAMPLES);
  EXPECT_EQ(wav_file.getSamplesRef().size(), NUM_SAMPLES);

  const auto wav_span = wav_file.getSamplesRef();
  kazoo::Fft::FftResults results;
  kazoo::Fft::performFftFrequency(wav_span, results);

  // std::cout << results;
  results.saveResultsToCsvFile(RESULTS_CSV_FILE);

  // constexpr double FREQ_EPSILON = 10.0;
  // EXPECT_NEAR(results.max_amplitude.frequency, SINE_FREQ_HZ, FREQ_EPSILON);

  // constexpr double AMP_EPSILON = 0.1;
  // EXPECT_NEAR(results.max_amplitude.amplitude, AMPLITUDE, AMP_EPSILON);
}

TEST(Fft_test, kazooA_sample) {
  const std::string RESULTS_CSV_FILE = "Fft_test.kazooA_sample.fft_results.csv";

  // Delete the file if it exists
  if (std::filesystem::exists(RESULTS_CSV_FILE)) {
    std::filesystem::remove(RESULTS_CSV_FILE);
  }
  ASSERT_FALSE(std::filesystem::exists(RESULTS_CSV_FILE));

  kazoo::WavFile wav_file;
  wav_file.read("kazooA_sample.test.wav");

  constexpr double GAUSSIAN_MEAN = 0.0;
  constexpr double GAUSSIAN_STD_DEV = 0.1;
  constexpr size_t NUM_SAMPLES = 400;

  kazoo::WaveTools::addGaussianNoise(wav_file, GAUSSIAN_MEAN, GAUSSIAN_STD_DEV,
                                     NUM_SAMPLES);
  EXPECT_EQ(wav_file.getNumSamples(), NUM_SAMPLES);
  EXPECT_EQ(wav_file.getSamplesRef().size(), NUM_SAMPLES);

  const auto wav_span = wav_file.getSamplesRef();
  kazoo::Fft::FftResults results;
  kazoo::Fft::performFftFrequency(wav_span, results);

  // std::cout << results;
  results.saveResultsToCsvFile(RESULTS_CSV_FILE);

  // constexpr double FREQ_EPSILON = 10.0;
  // EXPECT_NEAR(results.max_amplitude.frequency, SINE_FREQ_HZ, FREQ_EPSILON);

  // constexpr double AMP_EPSILON = 0.1;
  // EXPECT_NEAR(results.max_amplitude.amplitude, AMPLITUDE, AMP_EPSILON);
}
