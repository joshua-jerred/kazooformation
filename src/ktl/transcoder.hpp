/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file transcoder.hpp
/// @date 2025-01-21

#pragma once

#include <ktl/symbol_stream.hpp>

namespace kazoo {

template <typename Token_t>
class Transcoder {
 public:
  Transcoder(SymbolStream<Token_t>& binary_stream)
      : symbol_stream_{binary_stream} {}

  /// @brief Pops symbols from the front of the symbol stream and encodes them
  /// into the audio buffer.
  void encodeAvailableSymbols() {}

 private:
  SymbolStream<Token_t>& symbol_stream_;
};

}  // namespace kazoo