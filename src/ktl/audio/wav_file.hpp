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

    // Read the RIFF header
    constexpr std::array<char, 4> EXPECTED_RIFF_HEADER = {'R', 'I', 'F', 'F'};
    file.read(input_buffer.data(), EXPECTED_RIFF_HEADER.size());
    for (size_t i = 0; i < EXPECTED_RIFF_HEADER.size(); ++i) {
      if (input_buffer[i] != EXPECTED_RIFF_HEADER[i]) {
        throw std::runtime_error("Invalid RIFF header");
      }
    }

    // Read the file size
    file.read(input_buffer.data(), 4);
    const uint32_t file_size =
        *reinterpret_cast<uint32_t*>(input_buffer.data());
    std::cout << "File size: " << file_size << std::endl;
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
  struct WavHeader {
    // 0-3: "RIFF"
    static constexpr std::array<char, 4> RIFF = {'R', 'I', 'F', 'F'};
    // 4-7: File size - 4 bytes for RIFF,
  };
};

}  // namespace kazoo