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
#include <ktl/models/k2/k2_model.hpp>
#include <ktl/models/k3/k3_model.hpp>
#include <ktl/models/testing_model.hpp>
// #include <ktl/symbol_stream.hpp>

namespace kazoo {

static const model::Testing::Model TEST_MODEL{};
static const model::Binary::Model BIN_MODEL{};
static const model::K1Model::Model K1_MODEL{};
static const model::K2PeakModel::Model K2_PEAK_MODEL{};
static const model::K3ReasonableModel::K3Model K3_MODEL{};

const ISymbolModel& TranslationLayer::getStaticSymbolModel(const ModelType model_type) {
  switch (model_type) {
    case ModelType::TESTING:
      return TEST_MODEL;
    case ModelType::BINARY:
      return BIN_MODEL;
    case ModelType::K1_MODEL:
      return K1_MODEL;
    case ModelType::K2_PEAK_MODEL:
      return K2_PEAK_MODEL;
    case ModelType::K3_REASONABLE_MODEL:
      return K3_MODEL;
    default:
      KTL_ASSERT("Invalid model type");
      throw std::runtime_error("Invalid model type");
  }
}

void TranslationLayer::encode(bool pre_post_padding) {
  // Encode
  stats_.symbols_last_encoded = encoder_.encodeAvailableSymbols(
      tx_symbol_stream_, tx_audio_channel_, pre_post_padding);

  // Update stats
  stats_.num_bytes = tx_symbol_stream_.getNumBytes();
  stats_.num_bytes_encoded += stats_.symbols_last_encoded;
  stats_.audio_samples = tx_audio_channel_.getNumSamples();
}

void TranslationLayer::saveWav(const std::string& filename) {
  WavFile wav_file;
  wav_file.loadFromAudioChannel(tx_audio_channel_);
  wav_file.write(filename);
}

void TranslationLayer::playAudioBlocking(bool clear_audio_channel) {
  PulseAudio::Player::play(tx_audio_channel_);
  if (clear_audio_channel) {
    tx_audio_channel_.clear();
  }
}

void TranslationLayer::loadAndDecodeWav(const std::string& filename,
                                        std::vector<uint8_t>& data) {
  WavFile wav_file;
  wav_file.read(filename);
  wav_file.populateAudioChannel(tx_audio_channel_);
  getStaticSymbolModel(model_type_)
      .decodeAudioToSymbols(tx_audio_channel_, tx_symbol_stream_);

  constexpr bool POP_SYMBOLS = true;
  KTL_ASSERT(tx_symbol_stream_.populateBinaryStream(
      tx_binary_stream_, tx_symbol_stream_.getNumBytes(), POP_SYMBOLS));

  if (!tx_binary_stream_.isByteAligned()) {
    stats_.decoded_was_byte_aligned = false;
    tx_binary_stream_.pad();
  } else {
    stats_.decoded_was_byte_aligned = true;
  }

  data = tx_binary_stream_.getBytes(tx_binary_stream_.getNumBytes());
}

}  // namespace kazoo

std::ostream& operator<<(std::ostream& os, const kazoo::TranslationLayer::Stats& stats) {
  os << std::dec;
  os << "Bytes In TX Buffer: " << stats.num_bytes
     << ", Rx Bytes: " << stats.num_bytes_received
     << ", Symbols Last Encoded: " << stats.symbols_last_encoded
     << ", Audio Samples Encoded: " << stats.audio_samples
     << ", Decoded WAV Aligned: " << (stats.decoded_was_byte_aligned ? "Yes" : "No")
     << ", RX Audio Samples: " << stats.rx_audio_samples
     << ", TX Audio Samples: " << stats.tx_audio_samples
     << ", Num Frames Received: " << stats.num_frames_received
     << ", Pulse [latency: " << stats.pulse_audio_latency_ms
     << "ms, volume: " << stats.pulse_audio_volume << ", rms: " << stats.pulse_audio_rms
     << "]";
  return os;
}