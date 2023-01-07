#include "OpenAutoIt/UnsafeOperations.hpp"

#include <phi/compiler_support/compiler.hpp>

#if PHI_COMPILER_IS(CLANG_COMPAT) || PHI_COMPILER_IS(GCC_COMPAT)
#    define ATTRIBUTE_NO_UBSAN __attribute__((no_sanitize("undefined")))
#else
#    define ATTRIBUTE_NO_UBSAN /* Nothing */
#endif

namespace OpenAutoIt
{
    ATTRIBUTE_NO_UBSAN phi::i64 UnsafeAdd(phi::i64 lhs, phi::i64 rhs) noexcept
    {
        return lhs.unsafe() + rhs.unsafe();
    }

    ATTRIBUTE_NO_UBSAN phi::i64 UnsafeMinus(phi::i64 lhs, phi::i64 rhs) noexcept
    {
        return lhs.unsafe() - rhs.unsafe();
    }

    ATTRIBUTE_NO_UBSAN phi::i64 UnsafeMultiply(phi::i64 lhs, phi::i64 rhs) noexcept
    {
        return lhs.unsafe() * rhs.unsafe();
    }

    ATTRIBUTE_NO_UBSAN phi::i64 UnsafeDivide(phi::i64 lhs, phi::i64 rhs) noexcept
    {
        return lhs.unsafe() / rhs.unsafe();
    }
} // namespace OpenAutoIt
