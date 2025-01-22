/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <iostream>
#include <memory>
#include <span>

#include <ktl/binary_stream.hpp>
#include <ktl/model/binary_model.hpp>
#include <ktl/model/testing_model.hpp>
// #include <ktl/symbol_stream.hpp>

namespace kazoo {

class TranslationLayer {
 public:
  enum class ModelType {
    UNKNOWN = 0,
    TESTING,
    BINARY
  };

  TranslationLayer(const ModelType model = ModelType::BINARY)
      : model_type_(model) {}

  /// @brief Takes input data and processes it through the symbol stream.
  /// @param data - The input data to be processed.
  void addData(std::span<const uint8_t> data) {
    binary_stream_.addBytes(data);
    symbol_stream_.processBinaryStream(binary_stream_);
    stats_.num_bytes += symbol_stream_.getNumBytes();
  }

  void encode() {
    stats_.symbols_last_encoded =
        transcoder_.encodeAvailableSymbols(symbol_stream_, audio_channel_);
  }

  struct Stats {
    size_t num_bytes{0};
    size_t symbols_last_encoded{0};
  };
  Stats getStats() const { return stats_; }

 private:
  static const ISymbolModel& getStaticSymbolModel(ModelType model_type);

  const ModelType model_type_;
  std::reference_wrapper<const ISymbolModel> model_ref_{
      getStaticSymbolModel(model_type_)};
  model::Testing::Stream symbol_stream_{model_ref_};
  model::Testing::Transcoder transcoder_{model_ref_};

  BinaryStream binary_stream_;
  AudioChannel audio_channel_;

  Stats stats_;
};

}  // namespace kazoo