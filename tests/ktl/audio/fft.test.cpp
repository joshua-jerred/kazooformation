
#include <filesystem>

#include <testing.hpp>

#include <ktl/audio/fft.hpp>

TEST(Fft_test, basicTest) {
  kazoo::Fft fft;
  fft.compute();
}
