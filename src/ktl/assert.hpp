/// @file assert.hpp
/// @author Joshua Jerred (https://joshuajer.red)
/// @brief
/// @date 2025-01-16
/// @copyright Copyright (c) 2025

#pragma once

#include <string>
#include <stdexcept>

namespace kazoo {

/// @todo line number and file name via macro
void ktl_assert(bool eval, const std::string &msg);

}  // namespace kazoo