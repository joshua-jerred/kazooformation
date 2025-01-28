/// =*= kazooformation =*=
/// @author joshua
/// @date 1/28/25
/// @copyright Copyright (c) 2025

#pragma once

#include <cstdint>

namespace kazoo {

static constexpr uint32_t AUDIO_SAMPLE_RATE = 44100;

// 2^15 - 1 (16-bit PCM)
static constexpr double MAX_SAMPLE_AMPLITUDE = 32767.0;

}  // namespace kazoo