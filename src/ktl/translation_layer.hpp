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
#include <queue>
#include <span>
#include <string>
#include <thread>
#include <vector>

#include "audio/pulse_audio.hpp"
#include "audio/wav_file.hpp"

#include <ktl/binary_stream.hpp>
#include <ktl/deframer.hpp>
#include <ktl/ktl_frame.hpp>
#include <ktl/models/binary_model.hpp>
#include <ktl/models/k1_model.hpp>
#include <ktl/models/k2/k2_model.hpp>
#include <ktl/models/k3/k3_model.hpp>
#include <ktl/models/testing_model.hpp>
// #include <ktl/symbol_stream.hpp>

namespace kazoo {

class TranslationLayer {
  static constexpr size_t FRAME_PREAMBLE_SIZE = 2;
  static constexpr size_t FRAME_POSTAMBLE_SIZE = 2;

  /// @brief The volume threshold below which pulse audio is considered
  /// quiet/not receiving audio.
  static constexpr double MIN_VOLUME_THRESHOLD = 0.01;

 public:
  enum class ModelType {
    UNKNOWN = 0,
    TESTING,
    BINARY,
    K1_MODEL,
    K2_PEAK_MODEL,
    K3_REASONABLE_MODEL
  };

  TranslationLayer(const ModelType model)
      : model_type_(model) {
  }

  ~TranslationLayer() {
    if (is_listening_) {
      std::cout << "Warning: Stopping listening in destructor." << std::endl;
      stopListening();
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

    size_t num_frames_received{0};

    uint32_t pulse_audio_latency_ms{0};
    double pulse_audio_volume{0.0};
    double pulse_audio_rms{0.0};
    bool is_quiet = false;
  };

  Stats getStats() {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
  }

  void sendFrame(const KtlFrame& frame) {
    std::vector<uint8_t> frame_bytes =
        frame.encodeFrame(FRAME_PREAMBLE_SIZE, FRAME_POSTAMBLE_SIZE);
    addData(frame_bytes);
    encode();
    playAudioBlocking();
    tx_audio_channel_.clear();
  }

  /// @brief Takes input data and processes it through the symbol stream.
  /// @param data - The input data to be processed.
  void addData(std::span<const uint8_t> data) {
    tx_binary_stream_.addBytes(data);
    tx_symbol_stream_.processBinaryStream(tx_binary_stream_);
    KTL_ASSERT(tx_binary_stream_.getNumBits() <= 8);
    stats_.num_bytes = tx_symbol_stream_.getNumBytes();
  }

  void encode(bool pre_post_padding = false);

  void saveWav(const std::string& filename);

  void playAudioBlocking(bool clear_audio_channel = true);

  void loadAndDecodeWav(const std::string& filename, std::vector<uint8_t>& data);

  /// @brief Starts listening for audio input on the default audio device.
  void startListening() {
    KTL_ASSERT(!is_listening_);
    is_listening_ = true;
    listening_thread_ = std::thread(&TranslationLayer::listeningThreadFunc, this);
  }

  void stopListening() {
    KTL_ASSERT(is_listening_);
    is_listening_ = false;
    if (listening_thread_.joinable()) {
      listening_thread_.join();
    }
  }

  std::optional<KtlFrame> getReceivedFrame() {
    std::lock_guard<std::mutex> lock(received_frames_mutex_);
    if (received_frames_.empty()) {
      return std::nullopt;
    }
    KtlFrame frame = received_frames_.front();
    received_frames_.pop();
    return frame;
  }

  void setContinuousMode(bool continuous_mode) {
    is_continuous_mode_ = continuous_mode;
  }

 private:
  void listeningThreadFunc() {
    // std::cout << "Listening thread started." << std::endl;
    PulseAudio::Reader pulse_audio_reader{};
    AudioChannel rx_audio_channel;

    bool quiet_input = false;
    size_t quiet_iter = 0;

    while (is_listening_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));

      if (!pulse_audio_reader.process()) {
        continue;
      }
      std::cout << std::dec;
      // std::cout << pulse_audio_reader.getLatency() << "ms "
      // << pulse_audio_reader.getVolume() << " "
      // << pulse_audio_reader.getRms() << std::endl;
      stats_.pulse_audio_latency_ms = pulse_audio_reader.getLatency();
      stats_.pulse_audio_volume = pulse_audio_reader.getVolume();
      stats_.pulse_audio_rms = pulse_audio_reader.getRms();

      if (pulse_audio_reader.getVolume() < MIN_VOLUME_THRESHOLD) {
        if (!quiet_input) {
          // std::cout << "Quiet input..." << std::endl;
          quiet_input = true;
          quiet_iter = 0;
        } else {
          // allow the first one to pass through so we can flush the stream
          quiet_iter++;
          if (quiet_iter == 2) {
            rx_audio_channel.clear();
            std::cout << "Quiet input, clearing" << std::endl;
            stats_.is_quiet = true;
            continue;
          } else if (quiet_iter > 2) {
            continue;
          }
        }
        quiet_input = true;

        // stats_.rx_audio_samples = 0;
      } else {
        if (quiet_input) {
          // std::cout << "Not quiet anymore" << std::endl;
        }
        quiet_input = false;
        stats_.is_quiet = false;
        quiet_iter = 0;
      }

      model::K1Model::Stream k1_symbol_stream{model_ref_};
      model::K2PeakModel::Stream k2_symbol_stream{model_ref_};
      model::K3ReasonableModel::K3Stream k3_symbol_stream{model_ref_};

      ISymbolStream& rx_symbol_stream =
          model_type_ == ModelType::K1_MODEL
              ? static_cast<ISymbolStream&>(k1_symbol_stream)
          : model_type_ == ModelType::K2_PEAK_MODEL
              ? static_cast<ISymbolStream&>(k2_symbol_stream)
              : static_cast<ISymbolStream&>(k3_symbol_stream);

      stats_mutex_.lock();
      rx_audio_channel.addSamples(pulse_audio_reader.getAudioBuffer());
      stats_.rx_audio_samples = rx_audio_channel.getNumSamples();
      getStaticSymbolModel(model_type_)
          .decodeAudioToSymbols(rx_audio_channel, rx_symbol_stream);
      // rx_audio_channel.clear();
      stats_.num_bytes_received = rx_symbol_stream.getNumBytes();
      stats_mutex_.unlock();

      constexpr bool POP_SYMBOLS = false;
      BinaryStream rx_binary_stream;
      bool res = rx_symbol_stream.populateBinaryStream(
          rx_binary_stream, rx_symbol_stream.getNumBytes(), POP_SYMBOLS);
      // std::cout << "Binary stream size: " << rx_binary_stream.getNumBytes()
      // << " res: " << res << std::endl;

      // if (!rx_binary_stream.isByteAligned()) {
      // stats_.decoded_was_byte_aligned = false;
      // rx_binary_stream.pad();
      // } else {
      // stats_.decoded_was_byte_aligned = true;
      // }

      if (!is_continuous_mode_) {
        // non-continuous output, use deframer to receive full frames

        if (!rx_binary_stream.isByteAligned() && rx_binary_stream.getNumBytes() > 7) {
          // std::cout << "not byte aligned, padding" << std::endl;
          // continue;
          rx_binary_stream.pad();
        } else {
          continue;
        }

        // std::cout << "byte aligned" << std::endl;
        Deframer deframer{};
        if (deframer.processInput(rx_binary_stream)) {
          // std::cout << "!!!! we got a frame!" << std::endl;

          KtlFrame frame;
          deframer.getFrame(frame);
          received_frames_mutex_.lock();
          received_frames_.push(frame);
          received_frames_mutex_.unlock();
          stats_.num_frames_received++;  /// @todo not thread safe
          rx_audio_channel.clear();
          deframer.reset();
          /// @todo
        }
        // std::cout << std::endl;
      }
    }
    std::cout << "Listening thread stopped." << std::endl;
    // PulseAudio::Player::startListening();
  }

  static const ISymbolModel& getStaticSymbolModel(ModelType model_type);

  const ModelType model_type_;
  std::reference_wrapper<const ISymbolModel> model_ref_{
      getStaticSymbolModel(model_type_)};

  /// @todo This is preventing runtime model selection from working. I'm not going to get
  /// to automatic link establishment, so it doesn't matter all that much.
  model::K3ReasonableModel::K3Stream tx_symbol_stream_{model_ref_};
  model::K3ReasonableModel::K3Transcoder encoder_{model_ref_};

  BinaryStream tx_binary_stream_;
  AudioChannel tx_audio_channel_;

  // Listening
  std::atomic<bool> is_listening_{false};
  std::thread listening_thread_{};

  /// @brief If \c false then use the deframer, if \c true just output the bytes as ASCII
  /// as they come in.
  std::atomic<bool> is_continuous_mode_{false};

  std::mutex received_frames_mutex_;
  std::queue<KtlFrame> received_frames_;

  std::mutex stats_mutex_;
  Stats stats_;
};

}  // namespace kazoo

std::ostream& operator<<(std::ostream& os, const kazoo::TranslationLayer::Stats& stats);