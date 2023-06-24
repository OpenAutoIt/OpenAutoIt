#include "OpenAutoIt/UnsafeOperations.hpp"

#include <phi/compiler_support/compiler.hpp>
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>

#if PHI_COMPILER_IS(CLANG_COMPAT) || PHI_COMPILER_IS(GCC_COMPAT)
#    define ATTRIBUTE_NO_SAN __attribute__((no_sanitize("undefined")))
#else
#    define ATTRIBUTE_NO_SAN /* Nothing */
#endif

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

namespace OpenAutoIt
{
ATTRIBUTE_NO_SAN PHI_ATTRIBUTE_CONST phi::i64 UnsafeAdd(phi::i64 lhs, phi::i64 rhs)
{
    return lhs.unsafe() + rhs.unsafe();
}

ATTRIBUTE_NO_SAN PHI_ATTRIBUTE_CONST phi::i64 UnsafeMinus(phi::i64 lhs, phi::i64 rhs)
{
    return lhs.unsafe() - rhs.unsafe();
}

ATTRIBUTE_NO_SAN PHI_ATTRIBUTE_CONST phi::i64 UnsafeMultiply(phi::i64 lhs, phi::i64 rhs)
{
    return lhs.unsafe() * rhs.unsafe();
}

ATTRIBUTE_NO_SAN PHI_ATTRIBUTE_CONST phi::i64 UnsafeDivide(phi::i64 lhs, phi::i64 rhs)
{
    return lhs.unsafe() / rhs.unsafe();
}

ATTRIBUTE_NO_SAN PHI_ATTRIBUTE_CONST phi::i64 UnsafeNegate(phi::i64 value)
{
    return -value.unsafe();
}

} // namespace OpenAutoIt
