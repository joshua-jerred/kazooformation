
#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/wav_file.hpp>

TEST(WavFile_test, loadWav) {
  const std::string TEST_WAV_PATH = "wav_file_input.test.wav";

  kazoo::WavFile wav_file;
  wav_file.read(TEST_WAV_PATH);
}
