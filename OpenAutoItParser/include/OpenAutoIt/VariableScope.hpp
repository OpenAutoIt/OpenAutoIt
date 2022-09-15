#pragma once

#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

namespace OpenAutoIt
{
    enum class VariableScope
    {
        Auto,   // Default when not specified. Local inside a function scope otherwise Global
        Global, // Explicitly global
        Local,  // Explicitly local
    };

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wreturn-type")

    [[nodiscard]] PHI_ATTRIBUTE_PURE constexpr const char* enum_name(VariableScope scope) noexcept
    {
        switch (scope)
        {
            case VariableScope::Auto:
                return "Auto";
            case VariableScope::Global:
                return "Global";
            case VariableScope::Local:
                return "Local";
        }

        PHI_ASSERT_NOT_REACHED();
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

} // namespace OpenAutoIt
