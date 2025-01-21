/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file binary_stream.hpp
/// @date 2025-01-21

#pragma once

#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <deque>
#include <map>
#include <span>
#include <sstream>
#include <vector>

namespace kazoo {

/// @brief A bit/byte stream structure for the project.
/// @details This is a simple deque wrapper. First in, first out as unaligned
/// bits can be added to the stream.
/// (byte 0)[MSB/First Added]
///   -> (byte 1)
///   -> (byte 2)
///   -> ...
///   -> (byte n)[LSB/Last Added]
///
/// ()
class BinaryStream {
 public:
  BinaryStream() = default;

  void addBits(uint8_t input_bits, size_t num_bits) {
    if (num_bits > 8 || num_bits == 0) {
      throw std::invalid_argument("num_bits must be <= 8 and > 0");
    }

    // if the stream is byte aligned, we need to add a byte to the stream
    if (num_bits_in_buffer_ == 0) {
      stream_data_.push_back(0);
    }

    uint8_t &input_bit_buffer = stream_data_.back();

    if (num_bits + num_bits_in_buffer_ >= 8) {
      const size_t bits_to_add = 8 - num_bits_in_buffer_;
      const uint8_t mask = (1 << bits_to_add) - 1;
      const uint8_t bits_to_add_masked = input_bits & mask;
      input_bit_buffer |= bits_to_add_masked << num_bits_in_buffer_;
      num_bits_in_buffer_ = 0;

      const size_t remaining_bits = num_bits - bits_to_add;
      if (remaining_bits > 0) {
        addBits(input_bits >> bits_to_add, remaining_bits);
      }
    } else {
      input_bit_buffer |= input_bits << num_bits_in_buffer_;
      num_bits_in_buffer_ += num_bits;
    }
  }

  /// @brief Add bytes to the stream. If the stream is not byte aligned when
  /// this function is called, an exception will be thrown.
  /// @param input_bits - The bits to be added to the stream,
  /// @param auto_pad
  void addBytes(std::span<uint8_t> input_bytes) {
    if (!isByteAligned()) {
      throw std::runtime_error("Stream is not byte aligned");
    }

    for (const auto byte : input_bytes) {
      stream_data_.push_back(byte);
    }
  }

  size_t getNumBits() const {
    return stream_data_.size() * 8 - getNumUpspecifiedBits();
  }

  size_t getNumBytes() const { return stream_data_.size(); }

  bool isByteAligned() const { return num_bits_in_buffer_ == 0; }

  size_t getNumUpspecifiedBits() const {
    if (isByteAligned()) {
      return 0;
    }

    return 8 - num_bits_in_buffer_;
  }

  std::vector<uint8_t> getBytes(size_t num_bytes) {
    return std::vector<uint8_t>(stream_data_.begin(),
                                stream_data_.begin() + num_bytes);
  }

  const std::deque<uint8_t> &getStreamDataConst() const {
    // if (!isByteAligned()) {
    //   throw std::runtime_error("Stream is not byte aligned");
    // }

    return stream_data_;
  }

 private:
  std::deque<uint8_t> stream_data_{};

  size_t num_bits_in_buffer_{0};
};

}  // namespace kazoo

std::ostream &operator<<(std::ostream &os,
                         const kazoo::BinaryStream &binary_stream) {
  static constexpr bool HEX_MODE = false;
  static constexpr size_t DISPLAY_BYTE_WIDTH = 4;
  static constexpr size_t MAX_BYTES = 64;

  const auto stream_data = binary_stream.getStreamDataConst();

  size_t bytes_on_line = 0;
  size_t byte_index = 0;
  for (const auto byte : stream_data) {
    const bool is_last_byte = (stream_data.size() - byte_index - 1) == 0;

    if (HEX_MODE) {
      os << "|0x" << std::hex << static_cast<int>(byte);
    } else {
      os << "|0b";

      for (int i = 7; i >= 0; i--) {
        if (is_last_byte &&
            (8 - static_cast<int>(binary_stream.getNumUpspecifiedBits())) <=
                i) {
          os << "x";
        } else {
          os << ((byte >> i) & 1U) ? "1" : "0";
        }
      }
    }
    bytes_on_line++;

    if (bytes_on_line >= DISPLAY_BYTE_WIDTH) {
      os << "|" << std::endl;
    }

    byte_index++;
  }

  os << "|";

  return os;
}