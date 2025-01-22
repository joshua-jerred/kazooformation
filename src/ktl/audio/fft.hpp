/// =*= kazooformation =*=
/// @author joshua
/// @date 1/22/25
/// @copyright Copyright (c) 2025

#pragma once

#include <cmath>
#include <fftw3.h>

namespace kazoo {

/// @cite https://stackoverflow.com/a/22059819
class Fft {
 public:
  Fft() {
    // Initialize the FFTW library
    //    fftw_init_threads();
    //    fftw_plan_with_nthreads(4);
  }

  ~Fft() {
    // Clean up the FFTW library
    //    fftw_cleanup_threads();
  }

  void compute() {
    const int N = 64;

    fftw_complex in[N];
    fftw_complex out[N];
    fftw_complex in2[N];

    fftw_plan p;
    fftw_plan q;

    int i;

    /* prepare a cosine wave */
    for (i = 0; i < N; i++) {
      in[i][0] = cos(3 * 2 * M_PI * i / N);
      in[i][1] = 0;
    }

    /* forward Fourier transform, save the result in 'out' */
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    for (i = 0; i < N; i++)
      printf("freq: %3d %+9.5f %+9.5f I\n", i, out[i][0], out[i][1]);
    fftw_destroy_plan(p);

    /* backward Fourier transform, save the result in 'in2' */
    printf("\nInverse transform:\n");
    q = fftw_plan_dft_1d(N, out, in2, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(q);
    /* normalize */
    for (i = 0; i < N; i++) {
      in2[i][0] *= 1. / N;
      in2[i][1] *= 1. / N;
    }
    for (i = 0; i < N; i++)
      printf("recover: %3d %+9.5f %+9.5f I vs. %+9.5f %+9.5f I\n", i, in[i][0],
             in[i][1], in2[i][0], in2[i][1]);
    fftw_destroy_plan(q);

    fftw_cleanup();
  }
};

}  // namespace kazoo