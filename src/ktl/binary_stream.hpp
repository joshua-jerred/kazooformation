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

#include <ktl/assert.hpp>

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

  /// @brief Add individual bits to the stream.
  /// @details Right aligned, so the first bits added will be the least
  /// significant bits.
  /// ex: addBits(0b1010, 4) -> 0bxxxx1010, addBits(0b111, 3) -> 0bx1111010
  /// @param input_bits - The bits to be added to the stream, right aligned.
  /// @param num_bits - The number of bits to add to the stream, must be between
  /// 1 and 8 inclusive.
  void addBits(uint8_t input_bits, size_t num_bits) {
    ktl_assert(num_bits_in_output_buffer_ == 0, "Output buffer is not empty");
    if (num_bits > 8 || num_bits == 0) {
      throw std::invalid_argument("num_bits must be <= 8 and > 0");
    }

    // if the stream is byte aligned, we need to add a byte to the stream
    if (num_bits_in_input_buffer_ == 0) {
      stream_data_.push_back(0);
    }

    uint8_t &input_bit_buffer = stream_data_.back();

    if (num_bits + num_bits_in_input_buffer_ >= 8) {
      const size_t bits_to_add = 8 - num_bits_in_input_buffer_;
      const uint8_t mask = (1 << bits_to_add) - 1;
      const uint8_t bits_to_add_masked = input_bits & mask;
      input_bit_buffer |= bits_to_add_masked << num_bits_in_input_buffer_;
      num_bits_in_input_buffer_ = 0;

      const size_t remaining_bits = num_bits - bits_to_add;
      if (remaining_bits > 0) {
        addBits(input_bits >> bits_to_add, remaining_bits);
      }
    } else {
      input_bit_buffer |= input_bits << num_bits_in_input_buffer_;
      num_bits_in_input_buffer_ += num_bits;
    }
  }

  /// @brief Pop bits off the front, effectively the inverse of addBits.
  /// @param output - The output buffer to store the bits in. This becomes
  /// zeroed along with the number of bits being added.
  /// @param num_bits - The number of bits to pop off the front of the stream.
  /// @return \c true if there were enought bits to complete the operation,
  /// \c false otherwise.
  /// @exception You cannot pop bits off the front if input bit buffer is in
  /// use.
  bool popBits(uint8_t &output, size_t num_bits) {

  }

  /// @brief Add bytes to the stream. If the stream is not byte aligned when
  /// this function is called, an exception will be thrown.
  /// @param input_bits - The bits to be added to the stream,
  void addBytes(std::span<const uint8_t> input_bytes) {
    if (!isByteAligned()) {
      throw std::runtime_error("Stream is not byte aligned");
    }

    for (const auto byte : input_bytes) {
      stream_data_.push_back(byte);
    }
  }

  /// @brief Get the number of bits in the stream.
  /// @return The number of bits in the stream.
  size_t getNumBits() const {
    return stream_data_.size() * 8 - getNumUpspecifiedBits();
  }

  /// @brief Get the number of bytes in the stream. If the stream has spare
  /// bits that do not align to a byte, the number of bytes will be rounded up
  /// to the nearest byte.
  /// @return The number of bytes in the stream.
  size_t getNumBytes() const { return stream_data_.size(); }

  /// @brief Get the byte alignment status of the stream.
  /// @return \c true if the stream is byte aligned, \c false otherwise.
  bool isByteAligned() const { return num_bits_in_input_buffer_ == 0; }

  /// @brief If adding bits, this will return the number of bits that can be
  /// added to the stream before the stream is byte aligned.
  /// @return The number of missing bits to align the stream to a byte.
  size_t getNumUpspecifiedBits() const {
    if (isByteAligned()) {
      return 0;
    }
    return 8 - num_bits_in_input_buffer_;
  }

  /// @brief Get the next num_bytes bytes from the stream. Does not remove the
  /// bytes from the stream.
  /// @param num_bytes - The number of bytes to get
  /// @return A vector copy of the next num_bytes bytes from the stream.
  std::vector<uint8_t> getBytes(size_t num_bytes) {
    return std::vector<uint8_t>(stream_data_.begin(),
                                stream_data_.begin() + num_bytes);
  }

  /// @brief Get a const reference to the underlying stream data.
  /// @return A const reference to the underlying stream data.
  const std::deque<uint8_t> &getStreamDataConst() const { return stream_data_; }

  /// @brief If the last element is being used as a bit buffer, this will fill
  /// the remaining bits until the stream is byte aligned.
  /// @details artifically
  void pad() {
    if (num_bits_in_input_buffer_ != 0) {
      stream_data_.push_back(0);
      num_bits_in_input_buffer_ = 0;
    }
  }

 private:
  std::deque<uint8_t> stream_data_{};

  /// @details The number of bits in the 'bit input buffer'. If a user calls
  /// addBits(0bXx, num_bits), the buffer, which is the last byte in
  /// stream_date_, this variable will be the number of bits in that buffer.
  /// If it does not equal 0, then the stream is not byte aligned. Bits have
  /// to be added or pad() has to be called prior to other actions.
  size_t num_bits_in_input_buffer_{0};

  /// @details Same deal as above, but for the output buffer. This can only
  /// be allowed to be non-zero if the input buffer is zero. This is to
  /// be used for the getBits() method.
  size_t num_bits_in_output_buffer_{0};
};

}  // namespace kazoo

std::ostream &operator<<(std::ostream &os,
                         const kazoo::BinaryStream &binary_stream);