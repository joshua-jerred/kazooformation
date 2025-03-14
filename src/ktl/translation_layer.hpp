/// =*= kazooformation =*=
/// @author joshua
/// @date 1/21/25
/// @copyright Copyright (c) 2025

#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <thread>
#include <vector>

#include "audio/pulse_audio.hpp"
#include "audio/wav_file.hpp"

#include <ktl/binary_stream.hpp>
#include <ktl/models/binary_model.hpp>
#include <ktl/models/k1_model.hpp>
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

  ~TranslationLayer() {
    if (is_listening_) {
      std::cout << "Warning: Stopping listening in destructor." << std::endl;
      stopListening();
    }
  }

  /// @brief Takes input data and processes it through the symbol stream.
  /// @param data - The input data to be processed.
  void addData(std::span<const uint8_t> data) {
    tx_binary_stream_.addBytes(data);
    tx_symbol_stream_.processBinaryStream(tx_binary_stream_);
    KTL_ASSERT(tx_binary_stream_.getNumBits() <= 8);
    stats_.num_bytes = tx_symbol_stream_.getNumBytes();
  }

  void encode(bool pre_post_padding = false) {
    stats_.symbols_last_encoded = encoder_.encodeAvailableSymbols(
        tx_symbol_stream_, tx_audio_channel_, pre_post_padding);
    stats_.num_bytes = tx_symbol_stream_.getNumBytes();
    stats_.num_bytes_encoded += stats_.symbols_last_encoded;
    stats_.audio_samples = tx_audio_channel_.getNumSamples();
  }

  void saveWav(const std::string& filename) {
    WavFile wav_file;
    wav_file.loadFromAudioChannel(tx_audio_channel_);
    wav_file.write(filename);
  }

  void playAudioBlocking(bool clear_audio_channel = true) {
    PulseAudio::Player::play(tx_audio_channel_);
    if (clear_audio_channel) {
      tx_audio_channel_.clear();
    }
  }

  void loadAndDecodeWav(const std::string& filename,
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

  /// @brief Starts listening for audio input on the default audio device.
  void startListening() {
    KTL_ASSERT(!is_listening_);
    is_listening_ = true;
    listening_thread_ =
        std::thread(&TranslationLayer::listeningThreadFunc, this);
  }

  void stopListening() {
    KTL_ASSERT(is_listening_);
    is_listening_ = false;
    if (listening_thread_.joinable()) {
      listening_thread_.join();
    }
  }

  struct Stats {
    size_t num_bytes{0};

    // cumulative
    size_t num_bytes_encoded{0};
    size_t num_bytes_received{0};
    size_t symbols_last_encoded{0};
    size_t audio_samples{0};

    bool decoded_was_byte_aligned{false};

    size_t rx_audio_samples{0};
    size_t tx_audio_samples{0};
  };

  Stats getStats() {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
  }

 private:
  void listeningThreadFunc() {
    std::cout << "Listening thread started." << std::endl;
    PulseAudio::Reader pulse_audio_reader{};
    AudioChannel rx_audio_channel;

    while (is_listening_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      if (!pulse_audio_reader.process()) {
        continue;
      }

      std::cout << pulse_audio_reader.getLatency() << "ms "
                << pulse_audio_reader.getVolume() << " "
                << pulse_audio_reader.getRms() << std::endl;

      stats_mutex_.lock();
      rx_audio_channel.addSamples(pulse_audio_reader.getAudioBuffer());
      stats_.rx_audio_samples = rx_audio_channel.getNumSamples();
      getStaticSymbolModel(model_type_)
          .decodeAudioToSymbols(rx_audio_channel, rx_symbol_stream_);
      rx_audio_channel.clear();
      stats_.num_bytes_received = rx_symbol_stream_.getNumBytes();
      stats_mutex_.unlock();
    }
    std::cout << "Listening thread stopped." << std::endl;
    // PulseAudio::Player::startListening();
  }

  static const ISymbolModel& getStaticSymbolModel(ModelType model_type);

  const ModelType model_type_;
  std::reference_wrapper<const ISymbolModel> model_ref_{
      getStaticSymbolModel(model_type_)};

  model::K1Model::Stream tx_symbol_stream_{model_ref_};
  model::K1Model::Stream rx_symbol_stream_{model_ref_};
  model::K1Model::Transcoder encoder_{model_ref_};

  BinaryStream tx_binary_stream_;
  AudioChannel tx_audio_channel_;

  // Listening
  std::atomic<bool> is_listening_{false};
  std::thread listening_thread_{};

  std::mutex stats_mutex_;
  Stats stats_;
};

}  // namespace kazoo

std::ostream& operator<<(std::ostream& os,
                         const kazoo::TranslationLayer::Stats& stats);