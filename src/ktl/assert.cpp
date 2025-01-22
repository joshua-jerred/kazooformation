/// @file assert.cpp
/// @author Joshua Jerred (https://joshuajer.red)
/// @brief
/// @date 2025-01-16
/// @copyright Copyright (c) 2025

#include <stdexcept>

#include <ktl/assert.hpp>

void ktlast_(bool eval, const char *file, int line, const char *func,
             const char *eval_str) {
  if (eval) {
    return;
  }

  throw std::runtime_error("Assertion failed: " + std::string(eval_str) +
                           " in " + std::string(file) + ":" +
                           std::to_string(line) + " in " + func);
}