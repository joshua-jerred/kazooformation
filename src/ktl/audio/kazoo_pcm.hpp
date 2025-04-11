/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file kazoo_pcm.hpp
/// @date 2025-02-16

#pragma once

#include <array>
#include <cstdint>

namespace kazoo {

namespace pcm_data {

extern const std::array<int16_t, 4410> DOOT1_PCM;
extern const std::array<int16_t, 4410> DOOT2_PCM;
extern const std::array<int16_t, 4410> DOOT3_PCM;
extern const std::array<int16_t, 4410> DOOT4_PCM;

// k3 model samples
inline constexpr uint32_t SYMBOL_SIZE = 4410;
extern const std::array<int16_t, SYMBOL_SIZE> K3_DOOT_LOW;
extern const std::array<int16_t, SYMBOL_SIZE> K3_DOOT_HIGH;

// static const std::array<int16_t, 100> SYMBOL_1_SAMPLES;

}  // namespace pcm_data

}  // namespace kazoo