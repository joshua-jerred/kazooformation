/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file k2_pcm.hpp
/// @date 2025-03-14

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace kazoo {

namespace k2_pcm_data {

static constexpr size_t K2_SYMBOL_SIZE = 4410;

extern const std::array<int16_t, K2_SYMBOL_SIZE> KAZOO_A_PCM;
extern const std::array<int16_t, K2_SYMBOL_SIZE> KAZOO_D_PCM;
extern const std::array<int16_t, K2_SYMBOL_SIZE> KAZOO_E_PCM;
extern const std::array<int16_t, K2_SYMBOL_SIZE> KAZOO_G_PCM;

}  // namespace k2_pcm_data

}  // namespace kazoo