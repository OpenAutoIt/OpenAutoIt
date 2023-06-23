#pragma once

#include <phi/core/types.hpp>

namespace OpenAutoIt
{
// Add
[[nodiscard]] phi::i64 UnsafeAdd(phi::i64 lhs, phi::i64 rhs);

// Minus
[[nodiscard]] phi::i64 UnsafeMinus(phi::i64 lhs, phi::i64 rhs);

// Multiply
[[nodiscard]] phi::i64 UnsafeMultiply(phi::i64 lhs, phi::i64 rhs);

// Divide
[[nodiscard]] phi::i64 UnsafeDivide(phi::i64 lhs, phi::i64 rhs);
} // namespace OpenAutoIt
