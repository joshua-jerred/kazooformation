/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <ktl/symbol_model.hpp>

namespace kazoo::model {

enum class BinaryToken : uint32_t {
  UNKNOWN = 0,
  SYMBOL_0,
  SYMBOL_1,
  _SYMBOL_COUNT
};

class BinaryModel : public SymbolModel<BinaryToken> {
 public:
  using Token = BinaryToken;

  BinaryModel()
      : SymbolModel<BinaryToken>{
            {{BinaryToken::SYMBOL_0, 0}, {BinaryToken::SYMBOL_1, 1}}, 1} {}
};

}  // namespace kazoo::model