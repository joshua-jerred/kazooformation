/// @file assert.hpp
/// @author Joshua Jerred (https://joshuajer.red)
/// @brief
/// @date 2025-01-16
/// @copyright Copyright (c) 2025

#pragma once

#include <string>

/// @todo line number and file name via macro
void ktlast_(bool eval, const char *file, int line, const char *func,
             const char *eval_str);

/// @brief This is lazy, but it'll result in me checking more things.
#define KTL_ASSERT(eval) ktlast_(eval, __FILE_NAME__, __LINE__, __func__, #eval)