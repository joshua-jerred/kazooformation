/// =*= kazooformation =*=
/// @author Joshua Jerred
/// @date 1/28/25
/// @copyright Copyright (c) 2025

#pragma once

#include <optional>

namespace kazoo {

struct EncoderContext {
  double wave_angle = 0;
  std::optional<uint32_t> previous_symbol_token_id = std::nullopt;
};

}  // namespace kazoo