
#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/wav_file.hpp>
#include <ktl/audio/wave_tools.hpp>

TEST(WavFile_test, createFileWithTone) {
  // Known good test file
  const std::string TEST_WAV_PATH = "ex1_perfect.wav";

  kazoo::WavFile wav_file;

  constexpr size_t NUM_SAMPLES = 10'000;
  constexpr size_t SINE_FREQ_HZ = 800;
  constexpr double AMPLITUDE = 0.7;
  kazoo::WaveTools::generateSinWaveSamples(wav_file, SINE_FREQ_HZ, NUM_SAMPLES,
                                           AMPLITUDE);

  wav_file.write(TEST_WAV_PATH);

  // wav_file.getHeader().print();

  // EXPECT_EQ(wav_file.getNumSamples(), 40015);
  // EXPECT_NEAR(wav_file.getDurationSeconds(), 0.907, 0.001);
  // EXPECT_NEAR(wav_file.getDurationMilliseconds(), 907, 1);
}

TEST(WavFile_test, loadWav) {
  // Known good test file
  //
  const std::string TEST_WAV_PATH = "wav_file_input.test.wav";

  kazoo::WavFile wav_file;
  wav_file.read(TEST_WAV_PATH);

  // wav_file.getHeader().print();

  EXPECT_EQ(wav_file.getNumSamples(), 40015);
  EXPECT_NEAR(wav_file.getDurationSeconds(), 0.907, 0.001);
  EXPECT_NEAR(wav_file.getDurationMilliseconds(), 907, 1);
}

TEST(WavFile_test, saveAndLoadWav) {
  const std::string TEST_WAV_PATH = "tmp_test_file.test.wav";

  if (std::filesystem::exists(TEST_WAV_PATH)) {
    std::filesystem::remove(TEST_WAV_PATH);
  }
  EXPECT_FALSE(std::filesystem::exists(TEST_WAV_PATH));

  kazoo::WavFile wav_file;
  wav_file.addSample(0x7FFF);
  wav_file.addSample(0x0000);
  wav_file.addSample(0x8000);
  wav_file.addSample(0xFFFF);
  EXPECT_EQ(wav_file.getNumSamples(), 4);
  wav_file.write(TEST_WAV_PATH);
  // wav_file.getHeader().print();
  EXPECT_TRUE(std::filesystem::exists(TEST_WAV_PATH));

  kazoo::WavFile new_wav_file;
  new_wav_file.read(TEST_WAV_PATH);
  // new_wav_file.getHeader().print();
  EXPECT_EQ(new_wav_file.getNumSamples(), 4);
}