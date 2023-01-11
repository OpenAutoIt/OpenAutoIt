#pragma once

#include <phi/core/types.hpp>
#include <string_view>

namespace OpenAutoIt
{
    struct StackTraceEntry
    {
        // TODO: Currently unused
        std::string_view file;
        std::string_view function;
        phi::u64         line;
        phi::u64         column;
    };
} // namespace OpenAutoIt
