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

// Negate
[[nodiscard]] phi::i64 UnsafeNegate(phi::i64 value);

// Abs
[[nodiscard]] phi::i64 UnsafeAbs(phi::i64 value);

} // namespace OpenAutoIt
