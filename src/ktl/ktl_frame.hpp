/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file ktl_frame.hpp
/// @date 2025-03-13

#pragma once

#include <array>
#include <cstdint>
#include <span>

#include <common/assert.hpp>
namespace kazoo {

/// @brief A data frame for the KTL protocol, Inspired by AX.25.
/// @details The bytes `0xAAA5` can be used as infinite padding for symbol
/// and byte level alignment. The start of a frame is detected by the bytes
/// `0xAAAF`.
class KtlFrame {
 public:
  static constexpr size_t FRAME_HEADER_SIZE = 4;
  static constexpr uint8_t FRAME_START_BYTE_A = 0b11111000;
  static constexpr uint8_t FRAME_START_BYTE_B = 0b00000111;

  static constexpr uint8_t FRAME_ALIGN_BYTE = 0xA5;  // 0b10100101

  static constexpr size_t MAX_FRAME_DATA_SIZE = 50;

  static constexpr size_t FRAME_END_SIZE = 1;
  static constexpr uint8_t FRAME_END_BYTE_A = 0xFF;
  // static constexpr uint8_t FRAME_END_BYTE_B = 0xAA;  // infinite pad allowed

  static constexpr size_t MAX_FRAME_SIZE =
      FRAME_HEADER_SIZE + MAX_FRAME_DATA_SIZE + FRAME_END_SIZE;

  KtlFrame() = default;

  KtlFrame(const std::string &data) { setData(data); }

  KtlFrame(const std::span<uint8_t> data) { setData(data); }

  ~KtlFrame() = default;

  /// @brief
  /// @param preamble_size - Number of alignment bytes to add before the frame,
  /// must be divisible by 2.
  /// @param postamble_size - Number of alignment bytes to add after the frame,
  /// must be divisible by 2.
  /// @return
  std::vector<uint8_t> encodeFrame(size_t preamble_size = 0,
                                   size_t postamble_size = 0) const {
    // KTL_ASSERT(preamble_size % 2 == 0);
    // KTL_ASSERT(postamble_size % 2 == 0);
    std::vector<uint8_t> encoded_frame;

    for (size_t i = 0; i < preamble_size; ++i) {
      encoded_frame.push_back(FRAME_ALIGN_BYTE);
    }

    encoded_frame.push_back(FRAME_START_BYTE_A);
    encoded_frame.push_back(FRAME_START_BYTE_B);

    uint16_t frame_size = static_cast<uint16_t>(frame_data_field_.size());
    encoded_frame.push_back(static_cast<uint8_t>(frame_size >> 8));
    encoded_frame.push_back(static_cast<uint8_t>(frame_size & 0xFF));

    KTL_ASSERT(encoded_frame.size() == preamble_size + FRAME_HEADER_SIZE);

    for (const auto &c : frame_data_field_) {
      encoded_frame.push_back(c);
    }

    KTL_ASSERT(encoded_frame.size() <=
               preamble_size + FRAME_HEADER_SIZE + MAX_FRAME_DATA_SIZE);

    encoded_frame.push_back(FRAME_END_BYTE_A);
    // encoded_frame.push_back(FRAME_END_BYTE_B);

    for (size_t i = 0; i < postamble_size; ++i) {
      encoded_frame.push_back(FRAME_ALIGN_BYTE);
    }

    KTL_ASSERT(encoded_frame.size() <= MAX_FRAME_SIZE);
    return encoded_frame;
  }

  void setData(const std::span<uint8_t> data) {
    KTL_ASSERT(data.size() <= MAX_FRAME_DATA_SIZE);
    frame_data_field_.clear();
    for (const auto &c : data) {
      frame_data_field_.push_back(c);
    }
  }

  void setData(const std::string &data) {
    KTL_ASSERT(data.size() <= MAX_FRAME_DATA_SIZE);
    frame_data_field_.clear();
    for (const auto &c : data) {
      frame_data_field_.push_back(static_cast<uint8_t>(c));
    }
  }

  std::vector<uint8_t> getData() const { return frame_data_field_; }

  size_t getDataSize() const { return frame_data_field_.size(); }

 private:
  std::vector<uint8_t> frame_data_field_;
};

}  // namespace kazoo