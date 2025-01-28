/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#include "translation_layer.hpp"

#include <iostream>
#include <memory>
#include <span>

#include <ktl/binary_stream.hpp>
#include <ktl/models/binary_model.hpp>
#include <ktl/models/testing_model.hpp>
// #include <ktl/symbol_stream.hpp>

namespace kazoo {

static const model::Testing::Model TEST_MODEL{};
static const model::Binary::Model BIN_MODEL{};

const ISymbolModel& TranslationLayer::getStaticSymbolModel(
    const ModelType model_type) {
  switch (model_type) {
    case ModelType::TESTING:
      return TEST_MODEL;
    case ModelType::BINARY:
      return BIN_MODEL;
    default:
      throw std::runtime_error("Invalid model type");
  }
}

}  // namespace kazoo