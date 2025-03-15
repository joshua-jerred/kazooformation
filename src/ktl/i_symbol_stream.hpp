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

  /// @brief Clear/Reset the symbol stream.
  virtual void clear() = 0;

  virtual void addSymbolId(uint32_t symbol_id) = 0;

  virtual size_t getNumBytes() const = 0;

  virtual bool populateBinaryStream(BinaryStream &binary_stream,
                                    size_t num_bytes,
                                    bool pop_symbols = true) = 0;

  virtual std::pair<size_t, size_t> processBinaryStream(
      BinaryStream &binary_stream) = 0;

  virtual bool popSymbolTokenId(uint32_t &token_id) = 0;
};

}  // namespace kazoo
