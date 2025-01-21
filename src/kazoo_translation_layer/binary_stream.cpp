/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file binary_stream.hpp
/// @date 2025-01-21


#include <kazoo_translation_layer/binary_stream.hpp>


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

