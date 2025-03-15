/// =*= kazooformation =*=
/// @author Joshua Jerred
/// @date 1/28/25
/// @copyright Copyright (c) 2025

#pragma once

#include <optional>

namespace kazoo {

/// @brief Encoder context, used to store state information be
struct EncoderContext {
  double wave_angle = 0;

  std::optional<uint32_t> previous_symbol_token_id = std::nullopt;
};

}  // namespace kazoo