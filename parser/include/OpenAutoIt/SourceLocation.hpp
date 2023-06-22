#pragma once

#include <phi/core/types.hpp>

namespace OpenAutoIt
{
struct SourceLocation
{
    phi::u64 line_number{0u};
    phi::u64 column{0u};
};
} // namespace OpenAutoIt
