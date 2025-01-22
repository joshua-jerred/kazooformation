
#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/fft.hpp>
#include <ktl/audio/wav_file.hpp>

TEST(Fft_test, basicTest) {
  kazoo::WavFile wav_file;
  wav_file.addSineWaveSamples(2000, 0.5, 4000);
  EXPECT_EQ(wav_file.getNumSamples(), 4000);
  EXPECT_EQ(wav_file.getSamplesRef().size(), 4000);

  kazoo::Fft::performFft(wav_file);
}
