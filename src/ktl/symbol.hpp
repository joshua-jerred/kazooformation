/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file symbol.hpp
/// @date 2025-01-21

#pragma once

#include <cstdint>
#include <map>

namespace kazoo {

/// @brief A basic symbol structure for the project.a
/// @tparam Token_t - The token type for the symbol (enum class).
template <typename Token_t, size_t NumBits>
struct Symbol {
  const Token_t token;
  const uint32_t value;
};

}  // namespace kazoo