/// @file assert.cpp
/// @author Joshua Jerred (https://joshuajer.red)
/// @brief
/// @date 2025-01-16
/// @copyright Copyright (c) 2025

#include <ktl/assert.hpp>

void kazoo::ktl_assert(bool eval, const std::string &msg) {
  if (!eval) {
    throw std::runtime_error(msg);
  }
}

