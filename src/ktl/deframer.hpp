/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file deframer.hpp
/// @date 2025-03-13

#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

#include <ktl/binary_stream.hpp>
#include <ktl/ktl_frame.hpp>

#define DEBUG_FRAME_START_DETECT

namespace kazoo {

class Deframer {
 public:
  Deframer() = default;
  ~Deframer() = default;

  /// @brief Process the input binary stream in a stateful manner to extract
  /// frames from sequential stream inputs.
  /// @param binary_stream - The binary stream to process, this will pop bits
  /// from the stream.
  /// @return \c true if a frame extraction was completed/a new frame was found,
  /// \c false otherwise.
  bool processInput(BinaryStream &binary_stream) {
    bool result = false;
    if (state_ == State::FIND_FRAME_START) {
      result = findFrameStart(binary_stream);
    } else if (state_ == State::FIND_FRAME_SIZE) {
      result = findFrameSize(binary_stream);
    } else if (state_ == State::FIND_FRAME_DATA) {
      result = findFrameData(binary_stream);
    } else if (state_ == State::FIND_FRAME_END) {
      result = findFrameEnd(binary_stream);
    }

    return result;
  }

  void reset() {
    frame_size_ = 0;
    frame_data_.clear();
    changeState(State::FIND_FRAME_START);
  }

  void getFrame(KtlFrame &frame) {
    KTL_ASSERT(state_ == State::FOUND_FRAME);
    frame = KtlFrame(frame_data_);
  }

 private:
  enum class State {
    FIND_FRAME_START = 0,
    FIND_FRAME_SIZE = 1,
    FIND_FRAME_DATA = 2,
    FIND_FRAME_END = 3,
    FOUND_FRAME = 4
  };

  void changeState(State new_state) {
    // std::cout << "State change: " << static_cast<int>(new_state) << "\n";
    state_ = new_state;
  }

  bool findFrameStart(BinaryStream &binary_stream) {
    if (binary_stream.getNumBytes() < 2) {
      return false;
    }

#ifdef DEBUG_FRAME_START_DETECT
    std::cout << "detecting frame start with " << binary_stream.getNumBytes()
              << " bytes in the stream." << std::endl;
#endif

    bool byte_aligned = false;  // set to true if we find an alignment byte
    uint8_t byte_buffer = 0;
    // Pop the first byte right away
    KTL_ASSERT(binary_stream.popBits(byte_buffer, 8));
    while (binary_stream.getNumBytes() > 2) {
      if (byte_buffer == KtlFrame::FRAME_ALIGN_BYTE) {
        byte_aligned = true;
#ifdef DEBUG_FRAME_START_DETECT
        std::cout << "-Found alignment byte" << std::endl;
#endif
      } else if (byte_buffer == KtlFrame::FRAME_START_BYTE_A) {
#ifdef DEBUG_FRAME_START_DETECT
        std::cout << "--Found first byte of frame start" << std::endl;
#endif

        // The next byte must be the second byte of the frame start
        uint8_t next_byte = 0;
        KTL_ASSERT(binary_stream.popBits(next_byte, 8));
        if (next_byte == KtlFrame::FRAME_START_BYTE_B) {
#ifdef DEBUG_FRAME_START_DETECT
          std::cout << "---Found second byte of frame start" << std::endl;
#endif
          changeState(State::FIND_FRAME_SIZE);
          return findFrameSize(binary_stream);
        } else {
#ifdef DEBUG_FRAME_START_DETECT
          std::cout
              << "Byte was not the second byte of frame start, instead it was: "
              << std::bitset<8>(next_byte) << " ("
              << static_cast<int>(next_byte) << ", 0x" << std::hex
              << static_cast<int>(next_byte) << "), moving on. "
              << binary_stream.getNumBytes() << " bytes left in the stream."
              << std::endl;
#endif
        }
      } else {
#ifdef DEBUG_FRAME_START_DETECT
        std::cout
            << "Byte was not the first byte of frame start, instead it was: "
            << std::bitset<8>(byte_buffer) << " (" << std::hex
            << static_cast<int>(byte_buffer) << "), moving on. "
            << binary_stream.getNumBytes() << " bytes left in the stream."
            << std::endl;
#endif
      }

      // Pop the next bit and shift it into the byte buffer
      if (byte_aligned) {
        // If we're byte aligned, we can just pop the next byte
        KTL_ASSERT(binary_stream.popBits(byte_buffer, 8));
        byte_aligned = false;
      } else {
        auto bitOpt = binary_stream.popBit();
        if (!bitOpt.has_value()) {
          std::cout << "woop, outta bits" << std::endl;
          return false;
        }
        byte_buffer = (byte_buffer << 1) | static_cast<uint8_t>(bitOpt.value());
#ifdef DEBUG_FRAME_START_DETECT

        std::cout << "new byte_buffer: \n " << std::bitset<8>(byte_buffer)
                  << ", 0x" << std::hex << static_cast<int>(byte_buffer) << ", "
                  << static_cast<char>(byte_buffer) << "\n "
                  << std::bitset<8>(
                         static_cast<uint8_t>(KtlFrame::FRAME_START_BYTE_A))
                  << std::endl;
#endif
      }
    }

    return false;
  }

  bool findFrameSize(BinaryStream &binary_stream) {
#ifdef DEBUG_FRAME_START_DETECT
    std::cout << "findFrameSize" << binary_stream.getNumBytes()
              << " bytes in the stream." << std::endl;
#endif

    if (binary_stream.getNumBytes() < 3) {
      return false;
    }

    uint8_t msb = 0;
    KTL_ASSERT(binary_stream.popByte(msb));

    uint8_t lsb = 0;
    KTL_ASSERT(binary_stream.popByte(lsb));

    uint16_t frame_size = (static_cast<uint16_t>(msb) << 8) | lsb;

    if (frame_size > KtlFrame::MAX_FRAME_DATA_SIZE) {
      std::cout << "frame size too big, resetting" << std::endl;
      reset();
      return false;
    }

    frame_size_ = frame_size;

#ifdef DEBUG_FRAME_START_DETECT
    std::cout << "----Found frame size: " << std::dec << frame_size_
              << std::endl;
#endif

    changeState(State::FIND_FRAME_DATA);
    return findFrameData(binary_stream);
  }

  bool findFrameData(BinaryStream &binary_stream) {
    // std::cout << "findFrameData" << std::endl;
    KTL_ASSERT(frame_size_ <= KtlFrame::MAX_FRAME_DATA_SIZE);

    if (binary_stream.getNumBytes() <= frame_size_) {
#ifdef DEBUG_FRAME_START_DETECT
      std::cout << "not enough bytes for frame data" << std::endl;
#endif
      return false;  // Wait until we have enough bytes
    }

    for (size_t i = 0; i < frame_size_; ++i) {
      uint8_t byte = 0;
      KTL_ASSERT(binary_stream.popByte(byte));
      frame_data_.push_back(byte);
    }

#ifdef DEBUG_FRAME_START_DETECT
    std::cout << "-Found frame data" << std::endl;
#endif
    changeState(State::FIND_FRAME_END);
    return findFrameEnd(binary_stream);
  }

  bool findFrameEnd(BinaryStream &binary_stream) {
// Here we expect to find the end of the frame, if we don't, assume
// everything is wrong and reset.
#ifdef DEBUG_FRAME_START_DETECT
    std::cout << "findFrameEnd with " << binary_stream.getNumBytes()
              << " bytes in the stream." << std::endl;
#endif
    if (binary_stream.getNumBytes() < 1) {
      return false;  // Wait until we have enough bytes
    }
    uint8_t byte_buffer = 0;
    KTL_ASSERT(binary_stream.popByte(byte_buffer));
    if (byte_buffer == KtlFrame::FRAME_END_BYTE_A) {
#ifdef DEBUG_FRAME_START_DETECT
      std::cout << "<< Found frame end" << std::endl;
#endif
      changeState(State::FOUND_FRAME);
      return true;  // << Final state, frame found
    }

#ifdef DEBUG_FRAME_START_DETECT
    std::cout << "Did not find frame end. Instead found: "
              << std::bitset<8>(byte_buffer) << " (" << std::hex
              << static_cast<int>(byte_buffer) << ")" << std::endl;
#endif
    return false;
  }

  State state_{State::FIND_FRAME_START};

  /// @brief The size of the frame data field, set when the frame size is found.
  uint16_t frame_size_{0};

  std::vector<uint8_t> frame_data_{};
};

}  // namespace kazoo