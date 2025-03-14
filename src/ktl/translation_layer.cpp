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
#include <ktl/models/k1_model.hpp>
#include <ktl/models/testing_model.hpp>
// #include <ktl/symbol_stream.hpp>

namespace kazoo {

static const model::Testing::Model TEST_MODEL{};
static const model::Binary::Model BIN_MODEL{};
static const model::K1Model::Model K1_MODEL{};

const ISymbolModel& TranslationLayer::getStaticSymbolModel(
    const ModelType model_type) {
  switch (model_type) {
    case ModelType::TESTING:
      return TEST_MODEL;
    case ModelType::BINARY:
      return BIN_MODEL;
    case ModelType::K1_MODEL:
      return K1_MODEL;
    default:
      KTL_ASSERT("Invalid model type");
      throw std::runtime_error("Invalid model type");
  }
}

}  // namespace kazoo

std::ostream& operator<<(std::ostream& os,
                         const kazoo::TranslationLayer::Stats& stats) {
  os << "Bytes In TX Buffer: " << stats.num_bytes
     << ", Rx Bytes: " << stats.num_bytes_received
     << ", Symbols Last Encoded: " << stats.symbols_last_encoded
     << ", Audio Samples Encoded: " << stats.audio_samples
     << ", Decoded WAV Aligned: "
     << (stats.decoded_was_byte_aligned ? "Yes" : "No")
     << ", RX Audio Samples: " << stats.rx_audio_samples
     << ", TX Audio Samples: " << stats.tx_audio_samples;
  return os;
}