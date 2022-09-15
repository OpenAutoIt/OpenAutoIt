#pragma once

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

    PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wcovered-switch-default")

    [[nodiscard]] constexpr const char* enum_name(VariableScope scope) noexcept
    {
        switch (scope)
        {
            case VariableScope::Auto:
                return "Auto";
            case VariableScope::Global:
                return "Global";
            case VariableScope::Local:
                return "Local";

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_CLANG_SUPPRESS_WARNING_POP()

} // namespace OpenAutoIt
