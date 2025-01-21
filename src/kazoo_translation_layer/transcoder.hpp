/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file transcoder.hpp
/// @date 2025-01-21

#pragma once

#include <kazoo_translation_layer/symbol_stream.hpp>

namespace kazoo {

template <typename Token_t>
class Transcoder {
 public:
  Transcoder(SymbolStream<Token_t>& binary_stream)
      : symbol_stream_{binary_stream} {}

  void encodeAvailableSymbols() {}

 private:
  SymbolStream<Token_t>& symbol_stream_;
};

}  // namespace kazoo