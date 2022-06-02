#pragma once

#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
    [[nodiscard]] inline std::string indent_times(phi::usize indent) noexcept
    {
        std::string ret;

        for (; indent > 0u; --indent)
        {
            ret += "  ";
        }

        return ret;
    }
} // namespace OpenAutoIt
