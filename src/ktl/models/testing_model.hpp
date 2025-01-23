/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <ktl/encoder.hpp>
#include <ktl/symbol_model.hpp>
#include <ktl/symbol_stream.hpp>

namespace kazoo::model {

class Testing {
 public:
  enum class Token : uint32_t {
    UNKNOWN = 0,
    SYMBOL_0,
    SYMBOL_1,
    _SYMBOL_COUNT
  };

  class Model : public SymbolModel<Token> {
   public:
    Model()
        : SymbolModel<Token>{{{Token::SYMBOL_0, 0}, {Token::SYMBOL_1, 1}}, 1} {}
  };

  using Stream = SymbolStream<Token>;
  using Transcoder = Encoder<Token>;
};

}  // namespace kazoo::model