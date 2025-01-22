/// =*= kazooformation =*=
/// @author joshua
/// @date 1/22/25
/// @copyright Copyright (c) 2025

#pragma once

#include <cmath>
#include <fftw3.h>
#include <span>

namespace kazoo {

/// @cite https://stackoverflow.com/a/22059819
class Fft {
 public:
  Fft() = default;

  static void compute() {
    const int N = 64;
    std::array<fftw_complex, N> in;
    std::array<fftw_complex, N> out;

    int i;
    prepareCosWave(in);
    performFft(in, out);
    for (i = 0; i < N; i++)
      printf("freq: %3d %+9.5f %+9.5f I\n", i, out[i][0], out[i][1]);
  }

  static void performFft(std::span<fftw_complex> in,
                         std::span<fftw_complex> out) {
    const int N = in.size();
    if (in.size() != out.size()) {
      throw std::runtime_error("Input and output arrays must be the same size");
    }

    fftw_plan p;
    p = fftw_plan_dft_1d(N, in.data(), out.data(), FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_cleanup();
  }

  static void prepareCosWave(std::span<fftw_complex> in) {
    const int N = in.size();
    for (int i = 0; i < N; i++) {
      in[i][0] = cos(4 * 2 * M_PI * i / N);
      in[i][1] = 0;  // no imaginary part
    }
  }
};

}  // namespace kazoo