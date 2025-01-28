/// =*= kazooformation =*=
/// @author joshua
/// @date 1/22/25
/// @copyright Copyright (c) 2025

#pragma once

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <common/assert.hpp>
#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/audio_constants.hpp>

namespace kazoo {

/// @brief A class for reading and writing WAV files.
/// @cite https://github.com/joshua-jerred/WavGen
class WavFile : public IAudioChannel {
 public:
  WavFile() = default;

  void loadFromAudioChannel(IAudioChannel& audio_channel) {
    auto ac_ref = audio_channel.getSamplesRef();

    samples_ = std::vector<int16_t>(ac_ref.begin(), ac_ref.end());
  }

  void addSample(int16_t sample) override { samples_.push_back(sample); }

  /// @brief Read a WAV file from disk. Clears the current audio sample buffer.
  /// @param file_path - The path to the WAV file.
  void read(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + file_path);
    }

    // The input buffer for reading the headers
    std::array<char, 32> input_buffer = {};

    // Save the initial position
    std::streampos initial_position = file.tellg();

    // Read the header
    {
      // Read the RIFF header
      file.read(input_buffer.data(), WavHeader::RIFF.size());
      for (size_t i = 0; i < WavHeader::RIFF.size(); ++i) {
        if (input_buffer[i] != WavHeader::RIFF[i]) {
          throw std::runtime_error("Invalid RIFF header");
        }
      }

      // Read the file size
      file.read(input_buffer.data(), 4);
      const uint32_t file_size =
          *reinterpret_cast<uint32_t*>(input_buffer.data());

      // Read the WAVE header
      file.read(input_buffer.data(), WavHeader::WAVE.size());
      for (size_t i = 0; i < WavHeader::WAVE.size(); ++i) {
        if (input_buffer[i] != WavHeader::WAVE[i]) {
          throw std::runtime_error("Invalid RIFF header");
        }
      }

      // Read the fmt header
      file.read(input_buffer.data(), FormatChunk::FMT.size());
      for (size_t i = 0; i < FormatChunk::FMT.size(); ++i) {
        if (input_buffer[i] != FormatChunk::FMT.at(i)) {
          throw std::runtime_error("Invalid fmt header");
        }
      }

      // Read the format chunk size
      file.read(input_buffer.data(), 4);
      wav_header_.format_chunk.format_chunk_size =
          *reinterpret_cast<uint32_t*>(input_buffer.data());

      // Read the format code
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.format_code =
          *reinterpret_cast<uint16_t*>(input_buffer.data());

      // Read the number of channels
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.num_channels =
          *reinterpret_cast<uint16_t*>(input_buffer.data());

      // Read the sample rate
      file.read(input_buffer.data(), 4);
      wav_header_.format_chunk.sample_rate =
          *reinterpret_cast<uint32_t*>(input_buffer.data());

      // Read the byte rate
      file.read(input_buffer.data(), 4);
      wav_header_.format_chunk.byte_rate =
          *reinterpret_cast<uint32_t*>(input_buffer.data());

      // Read the block align
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.block_align =
          *reinterpret_cast<uint16_t*>(input_buffer.data());

      // Read the bits per sample
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.bits_per_sample =
          *reinterpret_cast<uint16_t*>(input_buffer.data());
    }

    {
      // Read the data header
      file.read(input_buffer.data(), WavHeader::DATA.size());
      for (size_t i = 0; i < WavHeader::DATA.size(); ++i) {
        if (input_buffer[i] != WavHeader::DATA[i]) {
          throw std::runtime_error("Invalid data header");
        }
      }

      // Read the data size
      file.read(input_buffer.data(), 4);
      wav_header_.data_chunk.data_size =
          *reinterpret_cast<uint32_t*>(input_buffer.data());

      // Save the data start position
      wav_header_.data_chunk.data_start_position = file.tellg();

      // Not sure if this is worth expanding on yet. Leaving this here as
      // a not-implemented warning.
      KTL_ASSERT(wav_header_.data_chunk.data_start_position == 44);

      // Read the audio data
      samples_.clear();
      samples_.resize(wav_header_.data_chunk.data_size / 2);
      file.read(reinterpret_cast<char*>(samples_.data()),
                wav_header_.data_chunk.data_size);
    }
  }

  void write(const std::string& file_path) {
    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + file_path);
    }

    // Calculate the file size
    wav_header_.resterToDefault();
    wav_header_.file_size = samples_.size() * 2 + WavHeader::HEADER_SIZE;
    wav_header_.data_chunk.data_size = samples_.size() * 2;

    file.clear();
    file.seekp(0, std::ios::beg);
    // It's a bit verbose, but it's writing the correct header.
    // file.write(WavHeader::RIFF.data(), WavHeader::RIFF.size());
    file << "RIFF";
    file.write(reinterpret_cast<const char*>(&wav_header_.file_size), 4);
    file.write(WavHeader::WAVE.data(), WavHeader::WAVE.size());
    file.write(FormatChunk::FMT.data(), FormatChunk::FMT.size());
    file.write(reinterpret_cast<const char*>(
                   &wav_header_.format_chunk.format_chunk_size),
               4);
    file.write(
        reinterpret_cast<const char*>(&wav_header_.format_chunk.format_code),
        2);
    file.write(
        reinterpret_cast<const char*>(&wav_header_.format_chunk.num_channels),
        2);
    file.write(
        reinterpret_cast<const char*>(&wav_header_.format_chunk.sample_rate),
        4);
    file.write(
        reinterpret_cast<const char*>(&wav_header_.format_chunk.byte_rate), 4);
    file.write(
        reinterpret_cast<const char*>(&wav_header_.format_chunk.block_align),
        2);
    file.write(reinterpret_cast<const char*>(
                   &wav_header_.format_chunk.bits_per_sample),
               2);
    file.write(WavHeader::DATA.data(), WavHeader::DATA.size());
    file.write(reinterpret_cast<const char*>(&wav_header_.data_chunk.data_size),
               4);

    wav_header_.data_chunk.data_start_position = file.tellp();

    for (const auto sample : samples_) {
      file.write(reinterpret_cast<const char*>(&sample), 2);
    }

    file.close();
  }

  struct FormatChunk {
    // 12-15: "fmt "
    static constexpr std::array<char, 4> FMT = {'f', 'm', 't', ' '};

    uint16_t bits_per_sample = 0;

    // 16-19: Format Chunk Size | 16 for PCM
    uint32_t format_chunk_size = 0;
    // 20-21: Format | 1 = PCM
    uint16_t format_code = 0;
    // 22-23: Number of Channels | 1 = Mono, 2 = Stereo
    /// @todo support for more than 2 channels
    uint16_t num_channels = 0;
    // 24-27: Sample Rate
    uint32_t sample_rate = 0;
    // 28-31: Byte Rate | Sample Rate * Num Channels * Bits Per Sample / 8
    uint32_t byte_rate = 0;
    // 32-33: Block Align | Num Channels * Bits Per Sample / 8
    /// @todo num_channels * bits_per_sample / 8 == 4?
    uint16_t block_align = 0;
    // 34-35: Bits Per Sample | 8, 16, 24, 32
    // static constexpr uint16_t bits_per_sample = 16;
  };
  static constexpr FormatChunk DEFAULT_FORMAT_CHUNK = {
      .bits_per_sample = 16,
      .format_chunk_size = 16,
      .format_code = 1,                 // 1 = PCM
      .num_channels = 1,                // 1 = Mono, 2 = Stereo
      .sample_rate = 44100,             // 44100 Hz
      .byte_rate = 44100 * 1 * 16 / 8,  // 44100 * 1 * 16 / 8
      .block_align = 4                  // 1 * 16 / 8
  };

  struct DataChunk {
    // 0-3: "data"
    static constexpr std::array<char, 4> DATA = {'d', 'a', 't', 'a'};
    // 4-7: Data size
    uint32_t data_size = 0;

    // Data start position
    std::streampos data_start_position;
  };

  struct WavHeader {
    static constexpr size_t HEADER_SIZE = 44;

    // 0-3: "RIFF"
    static constexpr std::array<char, 4> RIFF = {'R', 'I', 'F', 'F'};
    // 4-7: File size (not including 0-7)
    uint32_t file_size = 0;
    // 8-11: "WAVE"
    static constexpr std::array<char, 4> WAVE = {'W', 'A', 'V', 'E'};

    FormatChunk format_chunk{DEFAULT_FORMAT_CHUNK};

    DataChunk data_chunk{};

    void resterToDefault() {
      format_chunk = DEFAULT_FORMAT_CHUNK;
      data_chunk = {};
    }

    // n-n+3: "data"
    static constexpr std::array<char, 4> DATA = {'d', 'a', 't', 'a'};

    void print() const {
      std::cout << "File size: " << file_size << std::endl;
      std::cout << "Format chunk size: " << format_chunk.format_chunk_size
                << std::endl;
      std::cout << "Format code: " << format_chunk.format_code << std::endl;
      std::cout << "Number of channels: " << format_chunk.num_channels
                << std::endl;
      std::cout << "Sample rate: " << format_chunk.sample_rate << std::endl;
      std::cout << "Byte rate: " << format_chunk.byte_rate << std::endl;
      std::cout << "Block align: " << format_chunk.block_align << std::endl;
      std::cout << "Bits per sample: " << format_chunk.bits_per_sample
                << std::endl;
      std::cout << "Data size: " << data_chunk.data_size << std::endl;
      std::cout << "Audio data start position: "
                << data_chunk.data_start_position << std::endl;

      std::cout << "- Calculated Number of Samples: "
                << data_chunk.data_size / 2 << std::endl;
      std::cout << "- Calculated Audio Length: "
                << static_cast<double>(data_chunk.data_size) /
                       format_chunk.byte_rate
                << " seconds" << std::endl;
    }
  };

  /// @brief Get a reference to the header object.
  const WavHeader& getHeader() const { return wav_header_; }

  size_t getNumSamples() const { return samples_.size(); }

  double getDurationSeconds() const {
    return static_cast<double>(wav_header_.data_chunk.data_size) /
           wav_header_.format_chunk.byte_rate;
  }

  uint32_t getDurationMilliseconds() const {
    return static_cast<uint32_t>(getDurationSeconds() * 1000);
  }

  /// @deprecated WaveTools
  // void addSineWaveSamples(uint16_t frequency, double amplitude,
  //                         uint32_t samples) {
  //   const float offset =
  //       M_PI * frequency / wav_header_.format_chunk.sample_rate;
  //
  //   for (uint32_t i = 0; i < samples; i++) {  // For each sample
  //     wave_angle_ += offset;
  //     int16_t sample = static_cast<int16_t>((amplitude * sin(wave_angle_)) *
  //                                           MAX_SAMPLE_AMPLITUDE);
  //     addSample(sample);
  //     // std::cout << "sample: " << sample << std::endl;
  //
  //     if (wave_angle_ > M_PI) {
  //       wave_angle_ -= M_PI;
  //     }
  //   }
  // }

  const std::span<const int16_t> getSamplesRef() const override {
    return {samples_.data(), samples_.size()};
  }

 private:
  WavHeader wav_header_{};

  std::vector<int16_t> samples_{};

  double wave_angle_ = 0.0;
};

}  // namespace kazoo