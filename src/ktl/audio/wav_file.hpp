/// =*= kazooformation =*=
/// @author joshua
/// @date 1/22/25
/// @copyright Copyright (c) 2025

#pragma once

#include <filesystem>
#include <fstream>
#include <string>

namespace kazoo {

/// @brief A class for reading and writing WAV files.
/// @cite https://github.com/joshua-jerred/WavGen
class WavFile {
 public:
  WavFile() {}

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
      std::cout << "File size: " << file_size << std::endl;

      // Read the WAVE header
      file.read(input_buffer.data(), WavHeader::WAVE.size());
      for (size_t i = 0; i < WavHeader::WAVE.size(); ++i) {
        if (input_buffer[i] != WavHeader::WAVE[i]) {
          throw std::runtime_error("Invalid RIFF header");
        }
      }
      std::cout << "WAVE header" << std::endl;

      // Read the fmt header
      file.read(input_buffer.data(), WavHeader::FMT.size());
      for (size_t i = 0; i < WavHeader::FMT.size(); ++i) {
        if (input_buffer[i] != WavHeader::FMT[i]) {
          throw std::runtime_error("Invalid fmt header");
        }
      }
      std::cout << "fmt header" << std::endl;

      // Read the format chunk size
      file.read(input_buffer.data(), 4);
      wav_header_.format_chunk.format_chunk_size =
          *reinterpret_cast<uint32_t*>(input_buffer.data());
      std::cout << "Format chunk size: "
                << wav_header_.format_chunk.format_chunk_size << std::endl;

      // Read the format code
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.format_code =
          *reinterpret_cast<uint16_t*>(input_buffer.data());
      std::cout << "Format code: " << wav_header_.format_chunk.format_code
                << std::endl;

      // Read the number of channels
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.num_channels =
          *reinterpret_cast<uint16_t*>(input_buffer.data());
      std::cout << "Number of channels: "
                << wav_header_.format_chunk.num_channels << std::endl;

      // Read the sample rate
      file.read(input_buffer.data(), 4);
      wav_header_.format_chunk.sample_rate =
          *reinterpret_cast<uint32_t*>(input_buffer.data());
      std::cout << "Sample rate: " << wav_header_.format_chunk.sample_rate
                << std::endl;

      // Read the byte rate
      file.read(input_buffer.data(), 4);
      wav_header_.format_chunk.byte_rate =
          *reinterpret_cast<uint32_t*>(input_buffer.data());
      std::cout << "Byte rate: " << wav_header_.format_chunk.byte_rate
                << std::endl;

      // Read the block align
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.block_align =
          *reinterpret_cast<uint16_t*>(input_buffer.data());
      std::cout << "Block align: " << wav_header_.format_chunk.block_align
                << std::endl;

      // Read the bits per sample
      file.read(input_buffer.data(), 2);
      wav_header_.format_chunk.bits_per_sample =
          *reinterpret_cast<uint16_t*>(input_buffer.data());
      std::cout << "Bits per sample: "
                << wav_header_.format_chunk.bits_per_sample << std::endl;

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
      std::cout << "Data size: " << wav_header_.data_chunk.data_size
                << std::endl;

      // Save the data start position
      wav_header_.data_chunk.data_start_position = file.tellg();
      std::cout << "Data start position: "
                << wav_header_.data_chunk.data_start_position << std::endl;
    }
  }

  void write(const std::string& file_path) {
    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + file_path);
    }

    const auto initial_position = file.tellp();
    file.seekp(0, std::ios::beg);
    file << "RIFF";
  }

 private:
  struct FormatChunk {
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

  struct DataChunk {
    // 0-3: "data"
    static constexpr std::array<char, 4> DATA = {'d', 'a', 't', 'a'};
    // 4-7: Data size
    uint32_t data_size = 0;

    // Data start position
    std::streampos data_start_position;
  };

  struct WavHeader {
    // 0-3: "RIFF"
    static constexpr std::array<char, 4> RIFF = {'R', 'I', 'F', 'F'};
    // 4-7: File size (not including 0-7)
    uint32_t file_size = 0;
    // 8-11: "WAVE"
    static constexpr std::array<char, 4> WAVE = {'W', 'A', 'V', 'E'};
    // 12-15: "fmt "
    static constexpr std::array<char, 4> FMT = {'f', 'm', 't', ' '};

    FormatChunk format_chunk{};

    DataChunk data_chunk{};

    static constexpr FormatChunk DEFAULT_FORMAT_CHUNK = {
        .bits_per_sample = 16,
        .format_chunk_size = 16,
        .format_code = 1,                 // 1 = PCM
        .num_channels = 1,                // 1 = Mono, 2 = Stereo
        .sample_rate = 44100,             // 44100 Hz
        .byte_rate = 44100 * 1 * 16 / 8,  // 44100 * 1 * 16 / 8
        .block_align = 4                  // 1 * 16 / 8
    };

    // n-n+3: "data"
    static constexpr std::array<char, 4> DATA = {'d', 'a', 't', 'a'};
    // n+4-n+7: Data size
    uint32_t data_size = 0;

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
      std::cout << "Data size: " << data_size << std::endl;
      std::cout << "Audio data start position: "
                << data_chunk.data_start_position << std::endl;

      std::cout << "- Calculated Audio Length: "
                << static_cast<double>(data_size) / format_chunk.byte_rate
                << " seconds" << std::endl;
    }
  };

  WavHeader wav_header_{};
};

}  // namespace kazoo