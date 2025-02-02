/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file i_symbol_stream.hpp
/// @date 2025-02-02

#pragma once

#include <cstdint>

namespace kazoo {

/// @brief An interface for a symbol stream.
class ISymbolStream {
 public:
  virtual ~ISymbolStream() = default;

  virtual void addSymbolId(uint32_t symbol_id) = 0;
};

}  // namespace kazoo
