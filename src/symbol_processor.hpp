/// @file symbol.hpp
/// @author Joshua Jerred (https://joshuajer.red)
/// @brief
/// @date 2025-01-16
/// @copyright Copyright (c) 2025

#pragma once

#include <cstdint>

namespace kazoo {

enum class Token : uint32_t {
  UNKNOWN = 0,
  SYMBOL_ZERO,
  SYMBOL_ONE,
  _SYMBOL_COUNT
};

struct Symbol {
  const Token token;
  const uint32_t value;
  static constexpr uint32_t num_bits = 1; // 1 or 0
};

class SymbolProcessor {
public:
  SymbolProcessor() = default;
}

} // namespace kazoo