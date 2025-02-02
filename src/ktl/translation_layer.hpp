/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <iostream>
#include <memory>
#include <span>

#include "audio/wav_file.hpp"

#include <ktl/binary_stream.hpp>
#include <ktl/models/binary_model.hpp>
#include <ktl/models/testing_model.hpp>
// #include <ktl/symbol_stream.hpp>

namespace kazoo {

class TranslationLayer {
 public:
  enum class ModelType {
    UNKNOWN = 0,
    TESTING,
    BINARY,
    K1_MODEL
  };

  TranslationLayer(const ModelType model)
      : model_type_(model) {}

  /// @brief Takes input data and processes it through the symbol stream.
  /// @param data - The input data to be processed.
  void addData(std::span<const uint8_t> data) {
    binary_stream_.addBytes(data);
    symbol_stream_.processBinaryStream(binary_stream_);
    KTL_ASSERT(binary_stream_.getNumBits() <= 8);
    stats_.num_bytes = symbol_stream_.getNumBytes();
  }

  void encode() {
    stats_.symbols_last_encoded =
        encoder_.encodeAvailableSymbols(symbol_stream_, audio_channel_);
    stats_.num_bytes = symbol_stream_.getNumBytes();
    stats_.audio_samples = audio_channel_.getNumSamples();
  }

  void saveWav(const std::string& filename) {
    WavFile wav_file;
    wav_file.loadFromAudioChannel(audio_channel_);
    wav_file.write(filename);
  }

  void loadAndDecodeWav(const std::string& filename) {
    WavFile wav_file;
    wav_file.read(filename);
    wav_file.populateAudioChannel(audio_channel_);
    // encoder_.decodeAudioChannel(audio_channel_, symbol_stream_);
    // stats_.num_bytes = symbol_stream_.getNumBytes();
  }

  struct Stats {
    size_t num_bytes{0};
    size_t symbols_last_encoded{0};
    size_t audio_samples{0};
  };
  Stats getStats() const { return stats_; }

 private:
  static const ISymbolModel& getStaticSymbolModel(ModelType model_type);

  const ModelType model_type_;
  std::reference_wrapper<const ISymbolModel> model_ref_{
      getStaticSymbolModel(model_type_)};
  model::Testing::Stream symbol_stream_{model_ref_};
  model::Testing::Transcoder encoder_{model_ref_};

  BinaryStream binary_stream_;
  AudioChannel audio_channel_;

  Stats stats_;
};

}  // namespace kazoo