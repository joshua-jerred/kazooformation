/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file i_symbol_stream.hpp
/// @date 2025-02-02

#pragma once

#include <cstdint>

namespace kazoo {

class BinaryStream;

/// @brief An interface for a symbol stream.
class ISymbolStream {
 public:
  virtual ~ISymbolStream() = default;

  virtual void addSymbolId(uint32_t symbol_id) = 0;

  virtual size_t getNumBytes() const = 0;

  virtual bool populateBinaryStream(BinaryStream &binary_stream,
                                    size_t num_bytes,
                                    bool pop_symbols = true) = 0;
};

}  // namespace kazoo
